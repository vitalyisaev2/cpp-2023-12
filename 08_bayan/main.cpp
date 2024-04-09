#include <algorithm>
#include <boost/filesystem/path.hpp>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <vector>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include "checksum_computer.hpp"
#include "file_block_checksum_computer.hpp"
#include "file_crawler.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "produce help message")
        ("include,I", po::value<std::vector<std::string>>(), "directory to scan for duplicates")
        ("exclude,E", po::value<std::vector<std::string>>(), "directory to exclude from scanning")
        ("min-file-size", po::value<std::size_t>()->default_value(1), "minimal file size to scan in bytes, default - 1 byte")
        ("file-mask", po::value<std::string>(), "regexp to filter files, default - all files checked")
        ("block-size,S", po::value<std::size_t>()->default_value(1024), "size of block to compute checksum from the file's content, default - 1024 bytes")
        ("checksum-type", po::value<NBayan::EChecksumType>(), "hash algorithm, possible options:\n* CRC32");
    // clang-format on

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (std::exception const& x) {
        std::cerr << x.what() << std::endl;
        return -1;
    }

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (!vm.count("include")) {
        std::cout << "You must provide directories to look for duplicates via --include/-I flag" << std::endl;
        return -1;
    }

    auto blockChecksumStorage = std::make_shared<NBayan::TBlockChecksumStorage>();

    try {
        // Prepare crawler
        NBayan::TFileCrawler fileCrawler(
            NBayan::TFileBlockChecksumComputer(vm["block-size"].as<std::size_t>(),
                                               vm["checksum-type"].as<NBayan::EChecksumType>()),
            blockChecksumStorage);

        // Prepare list of root directories to start crawling from
        const auto included = vm["include"].as<std::vector<std::string>>();
        std::vector<boost::filesystem::path> roots;
        roots.reserve(included.size());
        std::transform(included.cbegin(), included.cend(), std::back_inserter(roots),
                       [](const std::string& incl) { return boost::filesystem::path(incl); });
        fileCrawler.Run(roots);
    } catch (std::exception const& x) {
        std::cerr << boost::diagnostic_information(x) << std::endl;
    }

    // Print results
    auto duplicates = blockChecksumStorage->GetDuplicates();
    for (const auto& group : duplicates.Groups) {
        for (const auto& path : group) {
            std::cout << path << std::endl;
        }
        std::cout << std::endl;
    }
}
