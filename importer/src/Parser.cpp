#include "../include/Parser.h"

namespace FBX {
    Mesh Parser::parseMesh() {
        const Node geometry = findNode(findNode(root, "Objects").value(), "Geometry").value();
        const auto rawVertices = std::any_cast<std::vector<double>>(
                findNode(geometry, "Vertices").value().properties[0]);
        assert(rawVertices.size() % 3 == 0);

        Mesh mesh;
        for (std::vector<double>::size_type i = 0; i < rawVertices.size(); i += 3)
            mesh.vertices.emplace_back(rawVertices[i], rawVertices[i + 1], rawVertices[i + 2]);

        return mesh;
    }

    std::optional<Node> Parser::findNode(const Node &node, const std::string &nodeId) {
        for (const Node &e : node.children)
            if (e.id == nodeId)
                return e;
        return {};
    }
}