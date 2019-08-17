#pragma once

#include "Span.h"
#include <vector>
#include <any>
#include <variant>

namespace FBX {
    struct Node {
        size_t propertyCount = 0;
        size_t propertyLength = 0;
        std::string id = "";
        std::vector<Node> children;
        std::vector<std::any> properties;
    };
}