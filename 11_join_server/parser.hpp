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
        TResult ParseTruncate(std::vector<std::string>& splits) const;
        TResult ParseIntersect(std::vector<std::string>& splits) const;
        TResult ParseDifference(std::vector<std::string>& splits) const;
        // TResult ParseBinaryTableCommand(std::vector<std::string>& splits, const std::string& cmd) const;

        template <typename T>
        TResult ParseBinaryTableCommand(std::vector<std::string>& splits, const std::string& commandName) const {
            // If no tables specified, use defaults to satisfy homework requirements
            if (splits.size() == 1) {
                return TParser::TResult{
                    .Status_ = TStatus::Success(),
                    .Cmd_ = T{.TableName1_ = "A", .TableName2_ = "B"},
                };
            }

            if (splits.size() == 3) {
                return TParser::TResult{
                    .Status_ = TStatus::Success(),
                    .Cmd_ = T{.TableName1_ = std::move(splits[1]), .TableName2_ = std::move(splits[2])},
                };
            }

            std::stringstream ss;
            ss << "ERR: invalid number of terms for command " << commandName << " : wanted 3, got " << splits.size();
            return TParser::TResult{.Status_ = TStatus::Error(ss.str()), .Cmd_ = std::nullopt};
        }

        std::pair<std::optional<std::vector<std::string>>, TStatus> SplitStringBySpace(const std::string& str) const;
    };
} //namespace NDatabase
