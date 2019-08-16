#include "../include/Parser.h"

namespace FBX {
    std::optional<Node> Parser::findNode(const Node &node, const std::string &nodeId) const {
        for (Node e : node.children)
            if (e.id == nodeId)
                return e;
        return {};
    }

    Mesh Parser::parseMesh() const {
        Mesh mesh;

        const Node geometry = findNode(findNode(root, "Objects").value(), "Geometry").value();
        const auto rawVertices = std::any_cast<std::vector<double>>(
                findNode(geometry, "Vertices").value().properties[0]);
        assert(rawVertices.size() % 3 == 0);
        for (std::vector<double>::size_type i = 0; i < rawVertices.size(); i += 3)
            mesh.vertices.emplace_back(rawVertices[i], rawVertices[i + 1], rawVertices[i + 2]);

        return mesh;
    }
}