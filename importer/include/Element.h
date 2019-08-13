#pragma once

namespace FBX {
    struct Element {
        unsigned int propertyCount;
        unsigned int propertyLength;
        std::string elementId;
        std::vector<Element> children;
    };
}