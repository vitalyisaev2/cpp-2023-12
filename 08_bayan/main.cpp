#include <algorithm>
#include <boost/filesystem/path.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <vector>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "block_checksum_storage.hpp"
#include "checksum_computer.hpp"
#include "file_block_checksum_computer.hpp"
#include "file_crawler.hpp"

namespace po = boost::program_options;

std::vector<boost::filesystem::path> StringsToPathes(const std::vector<std::string>& src) {
    std::vector<boost::filesystem::path> dst;
    dst.reserve(src.size());
    std::transform(src.cbegin(), src.cend(), std::back_inserter(dst),
                   [](const std::string& incl) { return boost::filesystem::path(incl); });
    return dst;
}

void runCrawler(const NBayan::TBlockChecksumStorage::TPtr& blockChecksumStorage, std::size_t blockSize,
                const std::vector<std::string> included, const std::vector<std::string> excluded,
                const NBayan::EChecksumType checksumType, bool recursive, std::size_t maxChecksumSize) {
    // Prepare list of root directories to start crawling from

    // Run crawler
    NBayan::TFileCrawler fileCrawler(NBayan::TFileBlockChecksumComputer(blockSize, checksumType), blockChecksumStorage,
                                     StringsToPathes(included), StringsToPathes(excluded), recursive, maxChecksumSize);

    fileCrawler.Run();
}

void printResults(const NBayan::TBlockChecksumStorage::TPtr& blockChecksumStorage) {
    auto duplicates = blockChecksumStorage->GetDuplicates();
    for (const auto& group : duplicates.Groups) {
        for (const auto& path : group) {
            std::cout << path << std::endl;
        }
        std::cout << std::endl;
    }
}

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
        ("checksum-type", po::value<std::string>()->default_value(NBayan::EChecksumTypeToString(NBayan::EChecksumType::CRC32)), "hash algorithm, possible options:\n* CRC32\n* MD5")
        ("recursive,R", po::bool_switch()->default_value(false), "enables recursive directory file search")
        ("log-level", po::value<int>()->default_value(2), "logging level from 0 (trace) to 5 (fatal), default 2 (info)");
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

    boost::log::core::get()->set_filter(boost::log::trivial::severity >= vm["log-level"].as<int>());

    try {
        // prepare args
        auto blockChecksumStorage = std::make_shared<NBayan::TBlockChecksumStorage>();
        const std::size_t blockSize = vm["block-size"].as<std::size_t>();
        const auto included = vm["include"].as<std::vector<std::string>>();
        const auto excluded = vm["exclude"].as<std::vector<std::string>>();
        const auto checksumType = NBayan::EChecksumTypeFromString(vm["checksum-type"].as<std::string>());
        const auto recursive = vm["recursive"].as<bool>();
        const auto minFileSize = vm["min-file-size"].as<std::size_t>();

        // parse results
        runCrawler(blockChecksumStorage, blockSize, included, excluded, checksumType, recursive, minFileSize);
        printResults(blockChecksumStorage);
    } catch (std::exception const& x) {
        std::cerr << x.what() << std::endl << boost::diagnostic_information(x) << std::endl;
    }
}
