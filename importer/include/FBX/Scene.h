#pragma once

#include <utility>
#include <vector>
#include "Model.h"
#include "Settings.h"

namespace FBX {
    struct Scene {
        Scene(std::vector<std::shared_ptr<Model>> models, const Settings settings) : models(models),
                                                                                     settings(settings) {}

        const std::vector<std::shared_ptr<Model>> models;
        const Settings settings;
    };
}