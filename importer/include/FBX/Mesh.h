#pragma once

#include <vector>
#include "Vector3.h"
#include "Face.h"

namespace FBX {
    struct Mesh {
        std::vector<Vector3> vertices;
        std::vector<Face> faces;
    };
}