#pragma once

#include <sstream>
#include <tuple>
#include <iostream>

namespace NPrintIP {
    template <class T>
    std::enable_if_t<std::is_integral_v<T>, std::string> PrintIP(T value) {
        std::stringstream ss;
        auto ip = reinterpret_cast<const unsigned char*>(&value);

        constexpr std::size_t totalBytes = sizeof(T);
        for (short i = totalBytes - 1; i >= 0; i--) {
            ss << int(ip[i]);
            if (i != 0) {
                ss << ".";
            }
        }

        return ss.str();
    }

    template <class T>
    std::enable_if_t<std::is_same<std::string, T>::value, std::string> PrintIP(const T& value) {
        return value;
    }

    template <typename T, class = void>
    struct is_iterable: std::false_type {};

    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T&>())), decltype(std::end(std::declval<T&>()))>>
        : std::true_type {};

    template <class T>
    std::enable_if_t<is_iterable<T>::value && !std::is_same<std::string, T>::value, std::string> PrintIP(T container) {
        std::stringstream ss;

        auto total = std::distance(std::begin(container), std::end(container));
        auto it = std::begin(container);
        while (total > 1) {
            ss << *it << ".";
            it++;
            total--;
        }
        ss << *it;

        return ss.str();
    }

    template <std::size_t I = 0, class... Tp>
    typename std::enable_if<I == sizeof...(Tp), std::string>::type PrintIP(const std::tuple<Tp...>&) {
        return "";
    }

    template <std::size_t I = 0, class... Tp>
        typename std::enable_if < I<sizeof...(Tp), std::string>::type PrintIP(const std::tuple<Tp...>& t) {
        if (I == 0) {
            return std::to_string(std::get<I>(t)) + PrintIP<I + 1, Tp...>(t);
        }

        return "." + std::to_string(std::get<I>(t)) + PrintIP<I + 1, Tp...>(t);
    }

} //namespace NSfinae
