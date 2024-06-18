#include "parser.hpp"
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
            // } else if (splits[0] == "TRUNCATE") {
            // } else if (splits[0] == "INTERSECT") {
            // } else if (splits[0] == "DIFFERENCE") {
        } else {
            std::stringstream ss;
            ss << "Unexpected command: " << splits[0];
            return TResult{.Status_ = TStatus::Error(ss.str())};
        };
    }

    TParser::TResult TParser::ParseInsert(std::vector<std::string>& splits) const {
        if (splits.size() != 4) {
            std::stringstream ss;
            ss << "invalid number of terms for command INSERT: wanted 4, got " << splits.size();
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
            ss << "invalid number of terms for command SELECT: wanted 2, got " << splits.size();
            return TParser::TResult{.Status_ = TStatus::Error(ss.str())};
        }

        return TParser::TResult{
            .Status_ = TStatus::Success(),
            .Cmd_ = TCmdSelect{.TableName_ = std::move(splits[1])},
        };
    }

    std::pair<std::optional<std::vector<std::string>>, TStatus> TParser::SplitStringBySpace(const std::string& str) const {
        std::istringstream iss(str);
        std::string word;
        std::vector<std::string> results;

        while (iss >> word) {
            results.push_back(word);
        }

        if (results.size() == 0) {
            std::stringstream ss;
            ss << "Could not split string '" << str << "' into tokens";
            auto status = TStatus::Error(ss.str());
            return std::make_pair(std::nullopt, std::move(status));
        }

        return std::make_pair(std::move(results), TStatus::Success());
    }

} //namespace NDatabase
