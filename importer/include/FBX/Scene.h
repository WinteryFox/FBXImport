#pragma once

#include <vector>
#include "Mesh.h"

namespace FBX {
    struct Scene {
        std::vector<Mesh> meshes;
    };
}