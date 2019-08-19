#include "../include/Parser.h"

namespace FBX {
    Mesh Parser::parseMesh() {
        const Node geometry = findNode(findNode(root, "Objects").value(), "Geometry").value();
        const auto rawVertices = std::get<std::vector<double>>(findNode(geometry, "Vertices").value().properties[0]);

        Mesh mesh;
        mesh.vertices.reserve(rawVertices.size() / 3);
        std::memcpy(mesh.vertices.data(), rawVertices.data(), rawVertices.size());

        return mesh;
    }

    std::optional<Node> Parser::findNode(const Node &node, const std::string &nodeId) {
        for (const Node &e : node.children)
            if (e.id == nodeId)
                return e;
        return {};
    }
}