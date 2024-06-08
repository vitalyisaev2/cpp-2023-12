#pragma once

#include "database.hpp"
#include "commands.hpp"

namespace NDatabase {

    class TParser {
    public:
        struct TResult {
            TStatus Status_;
            std::optional<TCmd> Cmd_;
        };

        TResult Handle(std::string&& line) const;

    private:
        TResult ParseInsert(std::vector<std::string>& splits) const;
        TResult ParseSelect(std::vector<std::string>& splits) const;
        std::vector<std::string> SplitStringBySpace(std::string&& str) const;
    };
} //namespace NDatabase
