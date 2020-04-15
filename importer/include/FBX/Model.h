#pragma once

#include "Object.h"
#include "Node.h"
#include "Material.h"

namespace FBX {
    struct Model : public Object {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;

        explicit Model(const Node &node) : Object(std::get<int64_t>(node.properties[0])) {

        }
    };
}