#pragma once

#include <cstdio>
#include <iostream>

namespace FBX {
    template<class T>
    struct Span {
        const T *const begin;
        const T *const end;

        Span(T *start, size_t arraySize) : begin(start), end(begin + arraySize) {}

        [[nodiscard]] size_t size() const {
            size_t size = sizeof(T);
            return (end - begin) / sizeof(T);
        }

        bool operator==(const Span<T> &other) {
            return std::equal(begin, end, other.begin, other.end);
        }

        bool operator==(const std::string_view &other) {
            if (other.size() != size() * sizeof(T))
                return false;

            return operator==({other.begin(), other.size() * sizeof(T)});
        }

        bool operator!=(const Span<T> &other) {
            return !operator==(other);
        }

        bool operator!=(const std::string_view &other) {
            return !operator==(other);
        }

        T &operator[](size_t i) {
            return begin[i];
        };

        const T &operator[](size_t i) const {
            return begin[i];
        };
    };
}