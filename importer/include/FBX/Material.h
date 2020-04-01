#pragma once

#include "Texture.h"

namespace FBX {
    struct Material : public Object {
        std::shared_ptr<Texture> texture = nullptr;

        explicit Material(const Node &node) :
                Object(std::get<int64_t>(node.properties[0])) {
            // TODO
        }
    };
}