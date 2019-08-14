#pragma once

#include <vector>
#include <any>

namespace FBX {
    struct Element {
        unsigned int propertyCount = 0;
        unsigned int propertyLength = 0;
        std::string id = "";
        std::vector<Element> children;
        std::vector<std::any> data;
    };
}