#pragma once

#include <cstdint>
#include <vector>

namespace FBX {
    struct Face {
        std::vector<uint32_t> indices;

        uint32_t operator[](size_t index) {
            return indices[index];
        }

        uint32_t operator[](size_t index) const {
            return indices[index];
        }
    };
}