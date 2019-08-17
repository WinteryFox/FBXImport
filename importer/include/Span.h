#pragma once

#include <cstdio>

namespace FBX {
    template<class T>
    struct Span {
        T *begin;
        T *end;

        T &operator[](size_t i) {
            return begin[i];
        };

        const T &operator[](size_t i) const {
            return begin[i];
        };
    };
}