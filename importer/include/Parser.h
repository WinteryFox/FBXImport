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

        Mesh parseMesh();

    private:
        const Node &root;

        static std::optional<Node> findNode(const Node &node, const std::string &nodeId);
    };
}