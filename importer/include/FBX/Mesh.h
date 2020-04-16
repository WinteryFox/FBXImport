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
        std::vector<Face> faces{};
        std::vector<Vector2> uvs{};

        explicit Mesh(const Node &node) : Object(std::get<int64_t>(node.properties[0])) {
            const auto fbxVertices = std::get<std::vector<double>>(findNodes(node, "Vertices")[0].properties[0]);
            const auto fbxIndices = std::get<std::vector<int32_t>>(
                    findNodes(node, "PolygonVertexIndex")[0].properties[0]);

            uint32_t indexCount = 0;
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

            const auto fbxUvNode = findNodes(node, "LayerElementUV")[0];
            const auto mappingType = std::get<std::string>(
                    findNodes(fbxUvNode, "MappingInformationType")[0].properties[0]);
            const auto referenceType = std::get<std::string>(
                    findNodes(fbxUvNode, "ReferenceInformationType")[0].properties[0]);
            const auto fbxUvs = std::get<std::vector<double>>(findNodes(fbxUvNode, "UV")[0].properties[0]);

            if (mappingType == "ByPolygonVertex") {
                if (referenceType == "IndexToDirect") {
                    const auto fbxUvIndices = std::get<std::vector<int32_t>>(
                            findNodes(fbxUvNode, "UVIndex")[0].properties[0]);
                    assert(fbxUvIndices.size() % 2 == 0);

                    uvs.reserve(fbxUvIndices.size());
                    for (const auto &index : fbxUvIndices)
                        uvs.emplace_back(fbxUvs[index * 2], 1.0f - fbxUvs[index * 2 + 1]);
                }
            } else {
                // TODO
            }
        }
    };
}