#pragma once

#include <vector>
#include "Vector3.h"

namespace FBX {
    struct Mesh {
        std::vector<Vector3> vertices;
    };
}