#pragma once

#include <vector>

namespace FBX {
    struct Mesh {
        std::vector<double> vertices;
        std::vector<double> normals;
        std::vector<uint32_t> indices;
    };
}