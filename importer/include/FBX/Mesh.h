#pragma once

#include <utility>
#include <vector>
#include "Vector3.h"
#include "Face.h"
#include "Material.h"

namespace FBX {
    struct Mesh : public Object {
        std::vector<Vector3> vertices{};
        std::vector<Face> faces{};
        std::shared_ptr<Material> material = nullptr;

        explicit Mesh(const Node &node) : Object(0) {
            const auto fbxVertices = std::get<std::vector<double>>(findNodes(node, "Vertices")[0].properties[0]);
            const auto fbxPolygons = std::get<std::vector<int32_t>>(
                    findNodes(node, "PolygonVertexIndex")[0].properties[0]);

            vertices.reserve(fbxVertices.size() / 3);
            for (size_t i = 0; i < fbxVertices.size(); i += 3)
                vertices.emplace_back(fbxVertices[i], fbxVertices[i + 1], fbxVertices[i + 2]);

            Face t;
            for (int32_t index : fbxPolygons) {
                if (index < 0) {
                    t.indices.push_back(-index - 1);
                    faces.push_back(t);
                    t = {};
                } else {
                    t.indices.push_back(index);
                }
            }
        }
    };
}