#pragma once

#include "Mesh.h"
#include <memory>

namespace FBX {
    struct Process {
        virtual ~Process() {};

        virtual void execute(const std::shared_ptr<Mesh> &mesh) = 0;
    };
}