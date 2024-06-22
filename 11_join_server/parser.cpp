#include "parser.hpp"
#include "commands.hpp"
#include "status.hpp"
#include <optional>
#include <utility>

namespace NDatabase {

    TParser::TResult TParser::Handle(const std::string& line) const {
        auto splitResult = SplitStringBySpace(line);
        if (!splitResult.second.Succeeded_) {
            return TParser::TResult{.Status_ = std::move(splitResult.second), .Cmd_ = std::nullopt};
        }

        auto& splits = *splitResult.first;

        if (splits[0] == "INSERT") {
            return ParseInsert(splits);
        } else if (splits[0] == "SELECT") {
            return ParseSelect(splits);
        } else if (splits[0] == "TRUNCATE") {
            return ParseTruncate(splits);
        } else if (splits[0] == "INTERSECT") {
            return ParseIntersect(splits);
        } else if (splits[0] == "DIFFERENCE") {
            return ParseDifference(splits);
        } else {
            std::stringstream ss;
            ss << "ERR: Unexpected command: " << splits[0];
            return TResult{.Status_ = TStatus::Error(ss.str())};
        };
    }

    TParser::TResult TParser::ParseInsert(std::vector<std::string>& splits) const {
        if (splits.size() != 4) {
            std::stringstream ss;
            ss << "ERR: invalid number of terms for command INSERT: wanted 4, got " << splits.size() << ": ";
            for (const auto& s : splits) {
                ss << "'" << s << "' ";
            }
            return TParser::TResult{.Status_ = TStatus::Error(ss.str())};
        }

        TRowData rowData(2);
        rowData.Append(std::stoi(splits[2])); // store id
        rowData.Append(std::move(splits[3])); // store str

        return TParser::TResult{
            .Status_ = TStatus::Success(),
            .Cmd_ = TCmdInsert{.TableName_ = std::move(splits[1]), .RowData_ = std::move(rowData)},
        };
    }

    TParser::TResult TParser::ParseSelect(std::vector<std::string>& splits) const {
        if (splits.size() != 2) {
            std::stringstream ss;
            ss << "ERR: invalid number of terms for command SELECT: wanted 2, got " << splits.size();
            return TParser::TResult{.Status_ = TStatus::Error(ss.str())};
        }

        return TParser::TResult{
            .Status_ = TStatus::Success(),
            .Cmd_ = TCmdSelect{.TableName_ = std::move(splits[1])},
        };
    }

    TParser::TResult TParser::ParseTruncate(std::vector<std::string>& splits) const {
        if (splits.size() != 2) {
            std::stringstream ss;
            ss << "ERR: invalid number of terms for command TRUNCATE: wanted 2, got " << splits.size();
            return TParser::TResult{.Status_ = TStatus::Error(ss.str())};
        }

        return TParser::TResult{
            .Status_ = TStatus::Success(),
            .Cmd_ = TCmdTruncate{.TableName_ = std::move(splits[1])},
        };
    }

    TParser::TResult TParser::ParseIntersect(std::vector<std::string>& splits) const {
        return ParseBinaryTableCommand<TCmdIntersect>(splits, "INTERSECT");
    }

    TParser::TResult TParser::ParseDifference(std::vector<std::string>& splits) const {
        return ParseBinaryTableCommand<TCmdDifference>(splits, "DIFFERENCE");
    }

    std::pair<std::optional<std::vector<std::string>>, TStatus>
    TParser::SplitStringBySpace(const std::string& str) const {
        if (!str.size()) {
            return std::make_pair(std::nullopt, TStatus::Error("Empty request"));
        }

        if (str[str.size() - 1] != '\n') {
            return std::make_pair(std::nullopt, TStatus::Error("Every request must be terminated with \\n symbol"));
        };

        std::istringstream iss(str);
        std::string word;
        std::vector<std::string> results;

        while (iss >> word) {
            if (word.size()) {
                results.push_back(word);
            }
        }

        if (results.size() == 0) {
            std::stringstream ss;
            ss << "ERR: cannot split string '" << str << "' into tokens";
            auto status = TStatus::Error(ss.str());
            return std::make_pair(std::nullopt, std::move(status));
        }

        return std::make_pair(std::move(results), TStatus::Success());
    }

} //namespace NDatabase
