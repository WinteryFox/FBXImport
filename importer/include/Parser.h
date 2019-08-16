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

        [[nodiscard]] Mesh parseMesh() const;

    private:
        const Node &root;

        [[nodiscard]] std::optional<Node> findNode(const Node &node, const std::string &nodeId) const;
    };
}