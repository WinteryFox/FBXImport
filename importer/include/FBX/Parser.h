#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <iostream>
#include <optional>
#include <memory>

#define _USE_MATH_DEFINES

#include <cmath>
#include <set>
#include "Node.h"
#include "Mesh.h"
#include "Scene.h"
#include "Process.h"
#include "Material.h"
#include "Util.h"
#include "Model.h"

namespace FBX {
    struct Parser {
        explicit Parser(const Node &root, const std::set<Process*> processes) : root(root), processes(processes) {}

        ~Parser();

        [[nodiscard]] std::unique_ptr<const Scene> parseScene() const;

        [[nodiscard]] std::shared_ptr<Mesh> parseMesh(const Node &node, Vector3 up) const;

    private:
        const Node &root;
        const std::set<Process*> processes;
    };
}