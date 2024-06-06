#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>

#include "database.hpp"

using boost::asio::ip::tcp;

class Session: public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket)
        : socket_(std::move(socket)) {
    }

    void start() {
        read();
    }

private:
    void read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        std::cout << "Received message: " << std::string(data_, length) << std::endl;
                                        write(length);
                                    }
                                });
    }

    void write(std::size_t length) {
        auto self(shared_from_this());
        std::string response = "Processed: " + std::string(data_, length);
        boost::asio::async_write(socket_, boost::asio::buffer(response),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec) {
                                         // Continue reading
                                         read();
                                     } else {
                                         std::cerr << "Write error: " << ec.message() << std::endl;
                                     }
                                 });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, NDatabase::TDatabase::TPtr database)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        accept();
    }

private:
    void accept() {
        acceptor_.async_accept([this](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            accept();
        });
    }

    NDatabase::TDatabase::TPtr database;
    tcp::acceptor acceptor_;
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

        Server s(io_context, std::atoi(argv[1]), NDatabase::TDatabase::Create());

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
