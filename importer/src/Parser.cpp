#include "../include/FBX/Parser.h"

namespace FBX {
    Scene Parser::parseScene() {
        const auto settings = findNodes(findNodes(root, "GlobalSettings")[0], "Properties70")[0];

        auto up = getProperty<int32_t>(settings, "UpAxis", 2);

        const auto objects = findNodes(root, "Objects")[0];
        const auto geometry = findNodes(objects, "Geometry");

        std::vector<Mesh> meshes;
        for (const auto &mesh : geometry) {
            if (isMesh(mesh)) {
                const auto &m = parseMesh(mesh, up);
                meshes.push_back(m);
            }
        }

        return Scene(meshes, up);
    }

    bool Parser::isMesh(const Node &node) {
        for (const auto &property : node.properties) {
            try {
                if (std::get<std::string>(property) == "Mesh")
                    return true;
            } catch (std::bad_variant_access &ignored) {}
        }
        return false;
    }

    template<class T>
    T Parser::getProperty(const Node &properties, const std::string &property, const T &fallback) {
        for (const auto &node : properties.children)
            if (std::get<std::string>(node.properties[0]) == property)
                return std::get<T>(node.properties[4]);
        return fallback;
    }

    Mesh Parser::parseMesh(const Node &node, const int32_t up) {
        const auto fbxVertices = std::get<std::vector<double>>(findNodes(node, "Vertices")[0].properties[0]);
        const auto fbxPolygons = std::get<std::vector<int32_t>>(
                findNodes(node, "PolygonVertexIndex")[0].properties[0]);

        Mesh mesh{};
        mesh.vertices.reserve(fbxVertices.size() / 3);
        for (size_t i = 0; i < fbxVertices.size(); i += 3)
            mesh.vertices.emplace_back(fbxVertices[i], fbxVertices[i + 1], fbxVertices[i + 2]);

        Face t;
        for (int32_t index : fbxPolygons) {
            if (index < 0) {
                t.indices.push_back(-index - 1);
                mesh.faces.push_back(t);
                t = {};
            } else {
                t.indices.push_back(index);
            }
        }

        if (processes & Process::TRIANGULATE) {
            /// Triangulate the mesh, currently only supports quads to triangles
            mesh.faces = triangulate(mesh.faces);
        } else {
            mesh.faces = mesh.faces;
        }

        for (auto &vertex : mesh.vertices) {
            Vector3 v = vertex;

            if (processes & MAKE_X_UP) {
                if (up == 1)
                    v = Vector3(vertex.y, vertex.x, vertex.z);
                else if (up == 2)
                    v = Vector3(vertex.z, vertex.y, vertex.x);
            } else if (processes & MAKE_Y_UP) {
                if (up == 0)
                    v = Vector3(vertex.y, vertex.x, vertex.z);
                else if (up == 2)
                    v = Vector3(vertex.x, vertex.z, vertex.y);
            } else if (processes & MAKE_Z_UP) {
                if (up == 0)
                    v = Vector3(vertex.z, vertex.y, vertex.x);
                else if (up == 1)
                    v = Vector3(vertex.x, vertex.z, vertex.y);
            }

            vertex = v;
        }

        return mesh;
    }

    std::vector<Node> Parser::findNodes(const Node &node, const std::string &nodeId) {
        std::vector<Node> nodes;

        for (const Node &e : node.children)
            if (e.id == nodeId)
                nodes.push_back(e);
        return nodes;
    }

    std::vector<Face> Parser::triangulate(const std::vector<Face> &faces) {
        std::vector<Face> triangles;

        for (const auto &face : faces) {
            /// Is this face a quad
            if (face.indices.size() == 4) {
                Face face1;
                Face face2;
                face1.indices.reserve(3);
                face2.indices.reserve(3);
                if (face[0] - face[2] < face[1] - face[3]) {
                    face1.indices.push_back(face[0]);
                    face1.indices.push_back(face[1]);
                    face1.indices.push_back(face[2]);

                    face2.indices.push_back(face[0]);
                    face2.indices.push_back(face[2]);
                    face2.indices.push_back(face[3]);
                } else {
                    face1.indices.push_back(face[0]);
                    face1.indices.push_back(face[1]);
                    face1.indices.push_back(face[3]);

                    face2.indices.push_back(face[3]);
                    face2.indices.push_back(face[1]);
                    face2.indices.push_back(face[2]);
                }
                triangles.push_back(face1);
                triangles.push_back(face2);
            } else {
                /// Face is not a quad, don't triangulate it
                triangles.push_back(face);
            }
        }

        return triangles;
    }
}