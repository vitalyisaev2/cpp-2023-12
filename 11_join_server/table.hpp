#include <tuple>
#include <string>
#include <deque>

namespace NJoin {
    using TTransactionId = std::size_t;

    class TRow {
        std::tuple<int, std::string> Data_;
    };

    struct TLogItem {
        enum class EEvent {
            Added = 0,
            Deleted = 1
        };

        TRow Row_;
        TTransactionId TranscationId_;
    };

    class TLog {
        std::deque<TLogItem> Rows_;
    };
} //namespace NJoin
