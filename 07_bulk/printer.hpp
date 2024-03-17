#pragma once

#include <memory>
#include <vector>
#include <string>

namespace NBulk {
    class TPrinter {
    public:
        using TPtr = std::unique_ptr<TPrinter>;

        void HandleBlock(const std::vector<std::string>& commands);
    };
} //namespace NBulk
