#pragma once

namespace FBX {
    struct Element {
        unsigned int propertyCount = 0;
        unsigned int propertyLength = 0;
        std::string elementId = "";
        std::vector<Element> children;
        std::vector<std::any> elementPropertiesData;
    };
}