#pragma once

#include <utility>
#include <vector>
#include "Model.h"

namespace FBX {
    struct Scene {
        Scene(std::vector<std::shared_ptr<Model>> models, const int32_t &up) : models(models),
                                                                                     up(up) {}

        const std::vector<std::shared_ptr<Model>> models;
        const Vector3 up;
    };
}