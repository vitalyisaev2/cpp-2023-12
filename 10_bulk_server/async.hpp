#pragma once

#include <cstddef>
#include <string>

namespace async {

    using handle_t = void*;

    handle_t connect(std::size_t bulk);
    void receive(handle_t handle, std::string&& input);
    void disconnect(handle_t handle);

} //namespace async
