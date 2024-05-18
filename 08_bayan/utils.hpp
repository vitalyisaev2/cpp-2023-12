#pragma once

#include <boost/filesystem/path.hpp>

namespace NBayan {
    struct TPathHasher {
        std::size_t operator()(const boost::filesystem::path& p) const noexcept {
            return std::hash<std::string>()(p.string());
        }
    };

} //namespace NBayan
