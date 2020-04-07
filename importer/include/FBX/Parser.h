#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <iostream>
#include <optional>
#include <memory>
#include "Node.h"
#include "Mesh.h"
#include "Scene.h"
#include "Process.h"
#include "Material.h"
#include "Util.h"
#include "Model.h"

namespace FBX {
    struct Parser {
        explicit Parser(const Node &root, int processes) : root(root), processes(processes) {}

        [[nodiscard]] std::unique_ptr<const Scene> parseScene() const;

        [[nodiscard]] std::shared_ptr<Mesh> parseMesh(const Node &node, int32_t up) const;

        static bool isMesh(const Node &node);

        std::vector<Face> static triangulate(const std::vector<Face> &faces);

    private:
        const Node &root;
        int processes;
    };
}