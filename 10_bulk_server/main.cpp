#include <boost/asio/io_context.hpp>

#include "server.hpp"

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
