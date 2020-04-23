#pragma once

#include <string>
#include "Util.h"
#include "Object.h"

namespace FBX {
    struct Texture : public Object {
        const std::string absolutePath;
        const std::string relativePath;

        explicit Texture(const Node &node) :
                Object(std::get<int64_t>(node.properties[0])),
                absolutePath(std::get<std::string>(findNodes(node, "FileName")[0].properties[0])),
                relativePath(std::get<std::string>(findNodes(node, "RelativeFilename")[0].properties[0])) {}
    };
}