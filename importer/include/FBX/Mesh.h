#pragma once

#include <utility>
#include <vector>
#include "Vector3.h"
#include "Vector2.h"
#include "Face.h"
#include "Material.h"

namespace FBX {
    struct Mesh : public Object {
        std::vector<Vector3> vertices{};
        std::vector<Face> faces{};
        std::vector<Vector2> uvs{};

        explicit Mesh(const Node &node) : Object(std::get<int64_t>(node.properties[0])) {
            const auto fbxVertices = std::get<std::vector<double>>(findNodes(node, "Vertices")[0].properties[0]);

            vertices.reserve(fbxVertices.size() / 3);
            for (size_t i = 0; i < fbxVertices.size(); i += 3)
                vertices.emplace_back(fbxVertices[i], fbxVertices[i + 1], fbxVertices[i + 2]);


            const auto fbxPolygons = std::get<std::vector<int32_t>>(
                    findNodes(node, "PolygonVertexIndex")[0].properties[0]);

            Face t;
            for (int32_t index : fbxPolygons) {
                if (index < 0) {
                    t.indices.push_back(~index);
                    faces.push_back(t);
                    t = {};
                } else {
                    t.indices.push_back(index);
                }
            }

            const auto fbxUvNode = findNodes(node, "LayerElementUV")[0];
            const auto mappingType = std::get<std::string>(
                    findNodes(fbxUvNode, "MappingInformationType")[0].properties[0]);
            const auto referenceType = std::get<std::string>(
                    findNodes(fbxUvNode, "ReferenceInformationType")[0].properties[0]);
            const auto fbxUvs = std::get<std::vector<double>>(findNodes(fbxUvNode, "UV")[0].properties[0]);
            const auto fbxUvIndices = std::get<std::vector<int32_t>>(findNodes(fbxUvNode, "UVIndex")[0].properties[0]);

            if (mappingType == "ByPolygonVertex") {
                if (referenceType == "IndexToDirect") {
                    assert(fbxUvIndices.size() % 2 == 0);
                    for (size_t i = 0; i < fbxUvIndices.size(); i += 2)
                        uvs.emplace_back(fbxUvs[fbxUvIndices[i] * 2], fbxUvs[fbxUvIndices[i + 1] * 2]);
                }
            } else {
                // TODO
            }

            std::cout << vertices.size() << std::endl;
            std::cout << uvs.size() << std::endl;
        }
    };
}