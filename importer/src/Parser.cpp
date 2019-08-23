#include "../include/Parser.h"

namespace FBX {
    Mesh Parser::parseMesh() {
        const Node geometry = findNode(findNode(root, "Objects").value(), "Geometry").value();
        const auto fbxVertices = std::get<std::vector<double>>(findNode(geometry, "Vertices").value().properties[0]);
        const auto fbxPolygons = std::get<std::vector<int32_t>>(
                findNode(geometry, "PolygonVertexIndex").value().properties[0]);

        Mesh mesh{};
        mesh.vertices.reserve(fbxVertices.size() / 3);

        mesh.indices.reserve(fbxPolygons.size());

        int32_t count = 0;
        for (int index : fbxPolygons) {
            const int absi = index < 0 ? (-index - 1) : index;
            if (static_cast<size_t>(absi) >= fbxVertices.size())
                throw std::runtime_error("Polygon vertex index out of range");

            mesh.vertices.push_back(fbxVertices[absi]);
            ++count;

            if (index < 0) {
                mesh.indices.push_back(count);
                count = 0;
            }
        }

        return mesh;
    }

    std::optional<Node> Parser::findNode(const Node &node, const std::string &nodeId) {
        for (const Node &e : node.children)
            if (e.id == nodeId)
                return e;
        return {};
    }
}