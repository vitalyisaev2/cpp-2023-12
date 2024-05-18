//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstddef>
#include <cstdlib>
#include <future>
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
        , Handle_(async::connect(bulkSize)) {
        std::cout << "Session created" << std::endl;
    }

    void Start() {
        std::cout << "Session started" << std::endl;
        DoRead();
    }

    ~TSession() {
        std::cout << "Session destroyed" << std::endl;
        async::disconnect(Handle_);
    }

private:
    void DoRead() {
        auto self(shared_from_this());
        Socket_.async_read_some(boost::asio::buffer(Data_, MaxLength_),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    std::cout << "Async read some" << std::endl;
                                    if (!ec) {
                                        async::receive(Handle_, Data_, length);
                                        DoWrite(length);
                                    }
                                });
    }

    void DoWrite(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(Socket_, boost::asio::buffer(Data_, length),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     std::cout << "Async write some" << std::endl;
                                     if (!ec) {
                                         DoRead();
                                     }
                                 });
    }

    tcp::socket Socket_;
    static const std::size_t MaxLength_ = 1024;
    char Data_[MaxLength_];
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

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
            return 1;
        }

        std::size_t port = std::atoi(argv[1]);
        std::size_t bulkSize = std::atoi(argv[2]);

        boost::asio::io_service ioService;
        TServer server(ioService, port, bulkSize);

        ioService.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
