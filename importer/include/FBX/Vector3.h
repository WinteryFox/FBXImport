#pragma once

#include <type_traits>

namespace FBX {
    template<
            class T,
            typename = typename std::enable_if<std::is_integral<T>::value, T>
    >
    struct Vector3 {
        T x = 0;
        T y = 0;
        T z = 0;

        Vector3() = default;

        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

        explicit Vector3(T init) : x(init), y(init), z(init) {}
    };
}