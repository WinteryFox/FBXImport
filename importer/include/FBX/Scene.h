#pragma once

#include <utility>
#include <vector>
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

namespace FBX {
    struct Scene {
        Scene(std::vector<std::shared_ptr<Mesh>> meshes, std::vector<std::shared_ptr<Texture>> textures,
              std::vector<std::shared_ptr<Material>> materials, const int32_t &up) : meshes(std::move(meshes)),
                                                                                     textures(std::move(textures)),
                                                                                     materials(std::move(materials)),
                                                                                     up(up) {}

        const std::vector<std::shared_ptr<Mesh>> meshes;
        const std::vector<std::shared_ptr<Texture>> textures;
        const std::vector<std::shared_ptr<Material>> materials;
        const Vector3 up;
    };
}