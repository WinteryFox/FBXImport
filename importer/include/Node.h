#pragma once

#include "Span.h"
#include <vector>
#include <variant>

namespace FBX {
    struct Node {
        size_t propertyCount = 0;
        size_t propertyLength = 0;
        std::string id = "";
        std::vector<Node> children;
        std::vector<std::variant<int16_t, bool, int32_t, float, double, int64_t, std::string, std::vector<float>, std::vector<int32_t>, std::vector<double>, std::vector<int64_t>, std::vector<bool>, std::vector<char>>> properties;
    };
}