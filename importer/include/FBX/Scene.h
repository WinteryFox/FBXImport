#pragma once

#include <utility>
#include <vector>
#include "Mesh.h"

namespace FBX {
    struct Scene {
        Scene(std::vector<Mesh> meshes, const int32_t &up) : meshes(std::move(meshes)), up(up) {}

        const std::vector<Mesh> meshes;
        const Vector3 up;
    };
}