#pragma once

#include <type_traits>

namespace FBX {
    template<
            class T,
            typename = std::enable_if_t<
                    std::is_same<T, double>::value ||
                    std::is_same<T, float>::value ||
                    std::is_same<T, int>::value, T>
    >
    struct Vector3 {
        T x = 0, y = 0, z = 0;

        Vector3() = default;

        explicit Vector3(T initializer) : x(initializer), y(initializer), z(initializer) {}

        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    };
}