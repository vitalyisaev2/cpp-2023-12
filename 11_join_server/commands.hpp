#pragma once

#include <variant>

#include "table.hpp"

namespace NDatabase {
    struct TCmdInsert {
        std::string TableName_;
        TRowData RowData_;
    };

    struct TCmdSelect {
        std::string TableName_;
    };

    struct TCmdTruncate {
        std::string TableName_;
    };

    struct TCmdIntersect {
        std::string TableName1_;
        std::string TableName2_;
    };

    struct TCmdDifference {
        std::string TableName1_;
        std::string TableName2_;
    };

    using TCmd = std::variant<TCmdInsert, TCmdSelect, TCmdTruncate, TCmdIntersect, TCmdDifference>;
} //namespace NDatabase
