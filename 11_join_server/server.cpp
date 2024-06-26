#include <boost/asio.hpp>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <memory>
#include <variant>
#include <vector>

#include "database.hpp"
#include "parser.hpp"
#include "status.hpp"
#include "table.hpp"

using boost::asio::ip::tcp;

class TSession: public std::enable_shared_from_this<TSession> {
public:
    TSession() = delete;

    TSession(tcp::socket socket, NDatabase::TDatabase::TPtr database)
        : Socket_(std::move(socket))
        , Parser_(NDatabase::TParser())
        , Database_(std::move(database)) 
        , Buffer_(BufferSize_, '\0')
        {
    }

    void Start() {
        DoRead();
    }

private:
    void DoRead() {
        auto self(shared_from_this());
        Socket_.async_read_some(boost::asio::buffer(self->Buffer_),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    std::cout << "DoRead: ec=" << ec.message() << ", length=" << length << std::endl;

                                    if (!ec) {
                                        std::cout << "Received request: '" << Buffer_ << "'" << std::endl;

                                        auto result = Parser_.Handle(std::string(Buffer_.data(), length));

                                        // Handle parser error
                                        if (!result.Status_.Succeeded_) {
                                            DoWriteStatus(std::move(result.Status_));
                                            return;
                                        }

                                        auto responseQueue = Database_->HandleCommand(std::move(*result.Cmd_));
                                        DoWriteQueueMessage(std::move(responseQueue));
                                    }
                                });
    }

    void DoWriteStatus(NDatabase::TStatus&& status) {
        auto self(shared_from_this());

        auto n = status.Dump(self->Buffer_);

        boost::asio::async_write(Socket_, boost::asio::buffer(self->Buffer_, n),
                                 [this, self](boost::system::error_code ec, std::size_t length) {
                                     std::cout << "DoWriteStatus: ec=" << ec.message() << ", length=" << length
                                               << std::endl;
                                 });
    }

    void DoWriteQueueMessage(NDatabase::TDatabase::TResultQueue::TPtr resultQueue) {
        auto self(shared_from_this());

        // handle next item from queue
        bool finished = false;
        std::size_t n;

        auto visitor = [&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::optional<NDatabase::TRowData>>) {
                n = arg->Dump(self->Buffer_);
            } else if constexpr (std::is_same_v<T, NDatabase::TStatus>) {
                n = arg.Dump(self->Buffer_);
                finished = true;
            } else {
                throw std::invalid_argument("unexpected message type");
            }
        };

        std::visit(visitor, resultQueue->Pop());

        boost::asio::async_write(Socket_, boost::asio::buffer(self->Buffer_, n),
                                 [this, self, finished, resultQueue](boost::system::error_code ec, std::size_t length) {
                                     std::cout << "DoWriteQueueMessage: ec=" << ec.message() << ", length=" << length
                                               << std::endl;

                                     if (!ec) {
                                         // handle next message or exit
                                         if (!finished) {
                                             DoWriteQueueMessage(resultQueue);
                                         }
                                     }
                                 });
    }

    tcp::socket Socket_;
    NDatabase::TParser Parser_;
    NDatabase::TDatabase::TPtr Database_;

    static const std::size_t BufferSize_ = 1024;
    std::string Buffer_;
};

class TServer {
public:
    TServer(boost::asio::io_context& io_context, short port, NDatabase::TDatabase::TPtr database)
        : Acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , socket(io_context)
        , Database_(std::move(database)) {
        accept();
    }

private:
    void accept() {
        Acceptor_.async_accept(socket, [this](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<TSession>(std::move(socket), Database_)->Start();
            }
            accept();
        });
    }

    tcp::acceptor Acceptor_;
    tcp::socket socket;
    NDatabase::TDatabase::TPtr Database_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        // 4 threads would be enough for this task
        int thread_count = std::thread::hardware_concurrency() < 4 ? std::thread::hardware_concurrency() : 4;

        std::cout << "Listening on localhost:" << argv[1] << std::endl;

        TServer s(io_context, std::atoi(argv[1]), NDatabase::TDatabase::Create());

        // Create a pool of threads to run all of the io_contexts
        std::vector<std::thread> threads;
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context]() { io_context.run(); });
        }

        // Wait for all threads in the pool to exit
        for (auto& t : threads) {
            t.join();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
