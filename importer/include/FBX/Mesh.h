#pragma once

#include <vector>
#include "Vector3.h"
#include "Vector2.h"
#include "Face.h"
#include "Object.h"
#include "Util.h"

namespace FBX {
    struct Mesh : public Object {
        std::vector<Vector3> vertices{};
        uint32_t indexCount = 0;
        std::vector<Face> faces{};
        std::vector<Vector2> uvs{};
        std::vector<Vector3> normals{};

        explicit Mesh(const Node &node) : Object(std::get<int64_t>(node.properties[0])) {
            const auto fbxVertices = std::get<std::vector<double>>(findNodes(node, "Vertices")[0].properties[0]);
            const auto fbxIndices = std::get<std::vector<int32_t>>(
                    findNodes(node, "PolygonVertexIndex")[0].properties[0]);

            Face face{};
            vertices.reserve(fbxIndices.size());
            for (const auto &i : fbxIndices) {
                int32_t index = i;
                if (index < 0) {
                    index = ~index;
                    face.indices.push_back(indexCount++);
                    faces.push_back(face);
                    face = {};
                } else {
                    face.indices.push_back(indexCount++);
                }

                vertices.emplace_back(
                        fbxVertices[index * 3],
                        fbxVertices[index * 3 + 1],
                        fbxVertices[index * 3 + 2]
                );
            }

            const auto &uvLayer = findNodes(node, "LayerElementUV");
            if (!uvLayer.empty())
                uvs = readLayer<Vector2, 2>(uvLayer[0], "UV");

            const auto &normalLayer = findNodes(node, "LayerElementNormal");
            if (!normalLayer.empty())
                normals = readLayer<Vector3, 3>(normalLayer[0], "Normals");
        }
    };
}