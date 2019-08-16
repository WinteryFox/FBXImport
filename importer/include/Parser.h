#pragma once

#include <cstring>
#include <string>
#include <iostream>
#include "Util.h"
#include "Element.h"
#include "Mesh.h"

namespace FBX {
    static Element findElement(const Element &element, const std::string &elementId) {
        for (Element e : element.children)
            if (e.id == elementId)
                return e;

    }

    static Mesh parseMesh(const Element &root) {
        Element mesh = findElement(findElement(root, "Objects"), "Geometry");

        const auto& vertices = std::any_cast<std::vector<float>>(findElement(mesh, "Vertices").properties[0]);

        return Mesh();
    }
}