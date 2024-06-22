#pragma once

#include <string>
#include <optional>

namespace NDatabase {
    // TStatus - a structure representing the final state of any operation.
    struct TStatus {
        bool Succeeded_;
        std::optional<std::string> Message_;

        static TStatus Success() {
            return TStatus{.Succeeded_ = true, .Message_ = std::nullopt};
        };

        static TStatus Error(std::string&& message) {
            return TStatus{.Succeeded_ = false, .Message_ = std::move(message)};
        };

        std::size_t Dump(std::string& buffer) const {
            if (Succeeded_) {
                const std::string OK = "OK";
                return OK.copy(buffer.data(), OK.size());
            }

            return Message_->copy(buffer.data(), Message_->size());
        }
    };
} //namespace NDatabase
