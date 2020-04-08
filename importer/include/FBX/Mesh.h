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
        std::vector<Vector3> uvs{};

        explicit Mesh(const Node &node) : Object(std::get<int64_t>(node.properties[0])) {
            const auto fbxVertices = std::get<std::vector<double>>(findNodes(node, "Vertices")[0].properties[0]);
            const auto fbxPolygons = std::get<std::vector<int32_t>>(
                    findNodes(node, "PolygonVertexIndex")[0].properties[0]);
            const auto fbxUvNode = findNodes(node, "LayerElementUV")[0];
            const auto fbxUvs = std::get<std::vector<double>>(findNodes(fbxUvNode, "UV")[0].properties[0]);
            const auto fbxUvIndices = std::get<std::vector<int32_t>>(findNodes(fbxUvNode, "UVIndex")[0].properties[0]);

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

            uvs.reserve(fbxUvs.size() / 3);
            for (size_t i = 0; i < fbxUvs.size(); i += 3)
                uvs.emplace_back(fbxUvs[i], fbxUvs[i + 1], fbxUvs[i + 2]);
        }
    };
}