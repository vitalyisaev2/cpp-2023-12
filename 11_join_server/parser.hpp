#pragma once

#include "commands.hpp"
#include "status.hpp"

namespace NDatabase {

    class TParser {
    public:
        struct TResult {
            TStatus Status_;
            std::optional<TCmd> Cmd_;
        };

        TResult Handle(const std::string& line) const;

    private:
        TResult ParseInsert(std::vector<std::string>& splits) const;
        TResult ParseSelect(std::vector<std::string>& splits) const;
        std::pair<std::optional<std::vector<std::string>>, TStatus> SplitStringBySpace(const std::string& str) const;
    };
} //namespace NDatabase
