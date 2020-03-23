#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <iostream>
#include <optional>
#include "Node.h"
#include "Mesh.h"
#include "Scene.h"
#include "Process.h"

namespace FBX {
    struct Parser {
        explicit Parser(const Node &root, int processes) : root(root), processes(processes) {}

        Scene parseScene();

        Mesh parseMesh(const Node &node, const int32_t up);

        template<class T>
        T getProperty(const Node &node, const std::string &property, const T &fallback);

        static bool isMesh(const Node &node);

        std::vector<Face> static triangulate(const std::vector<Face> &faces);

    private:
        const Node &root;
        int processes;

        static std::vector<Node> findNodes(const Node &node, const std::string &nodeId);
    };
}