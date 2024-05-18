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
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TSession: public std::enable_shared_from_this<TSession> {
public:
    TSession(tcp::socket socket)
        : Socket_(std::move(socket)) {
        std::cout << "Session created" << std::endl;
    }

    void Start() {
        std::cout << "Session started" << std::endl;
        DoRead();
    }

    ~TSession() {
        std::cout << "Session destroyed" << std::endl;
    }

private:
    void DoRead() {
        auto self(shared_from_this());
        Socket_.async_read_some(boost::asio::buffer(Data_, MaxLength_),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    std::cout << "Async read some" << std::endl;
                                    if (!ec) {
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
};

class TServer {
public:
    TServer(boost::asio::io_service& io_service, short port)
        : Acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
        , Socket_(io_service) {
        DoAccept();
    }

private:
    void DoAccept() {
        Acceptor_.async_accept(Socket_, [this](boost::system::error_code ec) {
            std::cout << "Server async accept" << std::endl;

            if (!ec) {
                std::make_shared<TSession>(std::move(Socket_))->Start();
            }

            DoAccept();
        });
    }

    tcp::acceptor Acceptor_;
    tcp::socket Socket_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_service ioService;

        TServer s(ioService, std::atoi(argv[1]));

        ioService.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
