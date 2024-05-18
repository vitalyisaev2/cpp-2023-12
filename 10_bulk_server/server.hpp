#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "async.hpp"

using boost::asio::ip::tcp;

class TSession: public std::enable_shared_from_this<TSession> {
public:
    TSession(tcp::socket socket, std::size_t bulkSize)
        : Socket_(std::move(socket))
        , Buffer_(BufferSize_, '\0') 
        , Handle_(async::connect(bulkSize))
        {
        std::cout << "Session created" << std::endl;
    }

    void Start() {
        std::cout << "Session started" << std::endl;
        DoRead();
    }

    ~TSession() {
        async::disconnect(Handle_);
        std::cout << "Session destroyed" << std::endl;
    }

private:
    void DoRead() {
        auto self(shared_from_this());
        Socket_.async_read_some(boost::asio::buffer(self->Buffer_),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    std::cout << "Async read some: " << length << " " << ec << std::endl;
                                    if (!ec) {
                                        for (auto& line : ExtractLinesFromBuffer()) {
                                            async::receive(Handle_, std::move(line));
                                        }
                                        DoRead();
                                    }
                                });
    }

    std::vector<std::string> ExtractLinesFromBuffer() {
        static const std::string delimiter("\n");
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = Buffer_.find(delimiter);

        while (end != std::string::npos) {
            tokens.push_back(Buffer_.substr(start, end - start));
            start = end + delimiter.length();
            end = Buffer_.find(delimiter, start);
        }
        // Add the last token
        tokens.push_back(Buffer_.substr(start));

        return tokens;
    }

    tcp::socket Socket_;
    static const std::size_t BufferSize_ = 1024;
    std::string Buffer_;
    async::handle_t Handle_;
};

class TServer {
public:
    TServer(boost::asio::io_service& io_service, short port, std::size_t bulkSize)
        : Acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
        , Socket_(io_service)
        , BulkSize_(bulkSize) {
        DoAccept();
    }

private:
    void DoAccept() {
        Acceptor_.async_accept(Socket_, [this](boost::system::error_code ec) {
            std::cout << "Server async accept" << std::endl;

            if (!ec) {
                std::make_shared<TSession>(std::move(Socket_), BulkSize_)->Start();
            }

            DoAccept();
        });
    }

    tcp::acceptor Acceptor_;
    tcp::socket Socket_;
    std::size_t BulkSize_;
};
