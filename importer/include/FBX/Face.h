#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace FBX {
    struct Face {
        std::vector<uint32_t> indices;

        Face() = default;

        explicit Face(std::vector<uint32_t> indices) : indices(std::move(indices)) {}

        uint32_t operator[](size_t index) {
            return indices[index];
        }

        uint32_t operator[](size_t index) const {
            return indices[index];
        }
    };
}