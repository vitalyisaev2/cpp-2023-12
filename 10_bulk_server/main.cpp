#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include "async.hpp"

using boost::asio::ip::tcp;

class TSession: public std::enable_shared_from_this<TSession> {
public:
    TSession(tcp::socket socket, std::size_t bulkSize)
        : Socket_(std::move(socket))
        , Buffer_(BufferSize_, '\0')
        , Handle_(async::connect(bulkSize)) {
    }

    void Start() {
        DoRead();
    }

    ~TSession() {
        async::disconnect(Handle_);
    }

private:
    void DoRead() {
        auto self(shared_from_this());
        Socket_.async_read_some(boost::asio::buffer(self->Buffer_),
                                [this, self](boost::system::error_code ec, std::size_t /*length*/) {
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
        , socket(io_service)
        , BulkSize_(bulkSize) {
        DoAccept();
    }

private:
    void DoAccept() {
        Acceptor_.async_accept(socket, [this](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<TSession>(std::move(socket), BulkSize_)->Start();
            }

            DoAccept();
        });
    }

    tcp::acceptor Acceptor_;
    tcp::socket socket;
    std::size_t BulkSize_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
            return 1;
        }

        std::size_t port = std::atoi(argv[1]);
        std::size_t bulkSize = std::atoi(argv[2]);

        boost::asio::io_context ioContext;
        TServer server(ioContext, port, bulkSize);

        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
            threads.emplace_back([&ioContext]() { ioContext.run(); });
        }

        for (auto& t : threads) {
            t.join();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
