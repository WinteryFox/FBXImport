#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <iostream>
#include <optional>
#include "Util.h"
#include "Node.h"
#include "Mesh.h"

namespace FBX {
    struct Parser {
        explicit Parser(const Node &root) : root(root) {}

        [[nodiscard]] const Mesh parseMesh() const;

    private:
        const Node &root;

        [[nodiscard]] const std::optional<Node> findNode(const Node &node, const std::string &nodeId) const;
    };
}