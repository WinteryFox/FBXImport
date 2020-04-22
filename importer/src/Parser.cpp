#include "../include/FBX/Parser.h"

namespace FBX {
    std::unique_ptr<const Scene> Parser::parseScene() const {
        const auto settings = findNodes(findNodes(root, "GlobalSettings")[0], "Properties70")[0];

        const auto up = getProperty<int32_t>(settings, "UpAxis", 2);
        const auto sign = getProperty<int32_t>(settings, "UpAxisSign", 1); // TODO

        const auto nodeObjects = findNodes(root, "Objects")[0];
        const auto nodeConnections = findNodes(root, "Connections")[0];
        const auto nodeModels = findNodes(nodeObjects, "Model");
        const auto nodeGeometry = findNodes(nodeObjects, "Geometry");
        const auto nodeMaterials = findNodes(nodeObjects, "Material");
        const auto nodeTextures = findNodes(nodeObjects, "Texture");

        std::vector<std::shared_ptr<Texture>> textures{};
        textures.reserve(nodeTextures.size());
        for (const auto &node : nodeTextures)
            textures.emplace_back(new Texture(node));

        std::vector<std::shared_ptr<Material>> materials{};
        materials.reserve(nodeMaterials.size());
        for (const auto &node : nodeMaterials)
            materials.emplace_back(new Material(node));

        std::vector<std::shared_ptr<Mesh>> meshes;
        for (const auto &node : nodeGeometry)
            if (isMesh(node))
                meshes.push_back(parseMesh(node, up));

        std::vector<std::shared_ptr<Model>> models;
        models.reserve(nodeModels.size());
        for (const auto &node : nodeModels)
            models.emplace_back(new Model(node));

        for (const auto &node : nodeConnections.children) {
            const auto source = std::get<int64_t>(node.properties[1]);
            const auto target = std::get<int64_t>(node.properties[2]);

            for (const auto &material : materials) {
                for (const auto &texture : textures)
                    if (source == texture->id && target == material->id)
                        material->texture = texture;

                for (const auto &model : models)
                    if (source == material->id && target == model->id)
                        model->material = material;
            }

            for (const auto &mesh : meshes)
                for (const auto &model : models)
                    if (source == mesh->id && target == model->id)
                        model->mesh = mesh;
        }

        return std::make_unique<Scene>(models, up);
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

    std::shared_ptr<Mesh> Parser::parseMesh(const Node &node, const int32_t up) const {
        std::shared_ptr<Mesh> mesh(new Mesh(node));

        if (processes & Process::TRIANGULATE)
            triangulate(mesh);

        for (auto &vertex : mesh->vertices) {
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

    bool Parser::isCounterClockwise(const Vector2 &a, const Vector2 &b, const Vector2 &c) {
        return (b.x - a.x) * (c.y - c.y) -
               (b.x - b.x) * (b.y - c.y) > 0;
    }

    void Parser::triangulate(const std::shared_ptr<Mesh> &mesh) {
        std::vector<Face> faces;

        for (auto &face : mesh->faces) {
            if (face.indices.size() <= 3) {
                faces.push_back(face);
            } else if (face.indices.size() == 4) {
                uint32_t startVertex = 0;
                for (uint32_t i = 0; i < 4; i++) {
                    const auto &v0 = mesh->vertices[face[(i + 3) % 4]];
                    const auto &v1 = mesh->vertices[face[(i + 2) % 4]];
                    const auto &v2 = mesh->vertices[face[(i + 1) % 4]];
                    const auto &v = mesh->vertices[face[i]];

                    const auto left = v0 - v;
                    const auto diag = v1 - v;
                    const auto right = v2 - v;

                    const float angle = std::acos(left * diag) + std::acos(right * diag);
                    if (angle > M_PI) {
                        startVertex = i;
                        break;
                    }
                }

                faces.emplace_back(
                        std::vector<uint32_t>{
                                face[startVertex],
                                face[(startVertex + 1) % 4],
                                face[(startVertex + 2) % 4]
                        }
                );
                faces.emplace_back(
                        std::vector<uint32_t>{
                                face[startVertex],
                                face[(startVertex + 2) % 4],
                                face[(startVertex + 3) % 4]
                        }
                );
            } else {
                std::vector<Vector3> vertices;
                vertices.reserve(face.indices.size());
                for (const auto i : face.indices)
                    vertices.push_back(mesh->vertices[i]);

                Vector3 normal{};
                for (const auto i : face.indices)
                    normal -= mesh->normals[i];
                normal.normalize();

                const float ax = (normal.x > 0 ? normal.x : -normal.x);
                const float ay = (normal.y > 0 ? normal.y : -normal.y);
                const float az = (normal.z > 0 ? normal.z : -normal.z);

                unsigned int ac = 0, bc = 1;
                float inv = normal.z;
                if (ax > ay) {
                    if (ax > az) {
                        ac = 1;
                        bc = 2;
                        inv = normal.x;
                    }
                } else if (ay > az) {
                    ac = 2;
                    bc = 0;
                    inv = normal.y;
                }

                if (inv < 0.0f)
                    std::swap(ac, bc);

                std::vector<Vector2> vertices2D;
                vertices2D.reserve(vertices.size());
                for (auto i = 0; i < vertices.size() - 1; i++)
                    vertices2D.emplace_back(vertices[i][ac], vertices[i][bc]);

                uint32_t earIndex = 0;
                Vector2 leftMost = vertices2D[0];
                for (uint32_t i = 0; i < vertices2D.size() - 1; i++) {
                    const auto vertex = vertices2D[i];
                    if (vertex.x > leftMost.x || (vertex.x == leftMost.x && vertex.y < leftMost.y)) {
                        leftMost = vertex;
                        earIndex = i;
                    }
                }

                const auto tri = Face(
                        std::vector<uint32_t>{
                                static_cast<unsigned int>(earIndex != 0 ? earIndex - 1 : vertices2D.size() - 1),
                                earIndex,
                                earIndex + 1 < vertices2D.size() ? earIndex + 1 : 0
                        }
                );

                uint32_t vertexCount = vertices2D.size();
                std::vector<uint32_t> reflex;
                uint32_t earTip = -1;
                while (vertexCount >= 3) {
                    for (uint32_t i = 0; i < vertices2D.size() - 1; i++) {
                        if (earTip >= 0)
                            break;

                        Vector2 prev = vertices2D[i - 1 > 0 ? i - 1 : vertices2D.size() - 1];
                        Vector2 current = vertices2D[i];
                        Vector2 next = vertices2D[i + 1 < vertices2D.size() ? i + 1 : 0];

                        if (isCounterClockwise(prev, current, next)) {
                            reflex.push_back(i);
                            continue;
                        }

                        bool ear = true;
                        for (uint32_t index : reflex) {
                            if (face[index] == tri[0] || face[index] == tri[2])
                                continue;

                            if (isInTriangle(vertices2D[face[index]], vertices2D[tri[0]], vertices2D[tri[1]], vertices2D[tri[2]])) {
                                ear = false;
                                break;
                            }
                        }

                        if (ear) {
                            for (uint32_t j = i + 1; i < vertexCount - 1; i++) {
                                if (face[j] == tri[0] || face[j] == tri[2])
                                    continue;

                                if (isInTriangle(vertices2D[face[j]], vertices2D[tri[0]], vertices2D[tri[1]],
                                                 vertices2D[tri[2]])) {
                                    ear = false;
                                    break;
                                }
                            }
                        }

                        if (ear)
                            earTip = i;
                    }

                    if (earTip < 0)
                        break;

                    uint32_t prev = earTip - 1 >= 0 ? earTip - 1 : vertexCount - 1;
                    uint32_t next = earTip + 1 < vertexCount ? earTip + 1 : 0;
                    faces.emplace_back(
                            std::vector<uint32_t>{
                                    prev, earTip, next
                            }
                    );
                    std::erase(face.indices, earTip);
                    vertexCount--;
                }
            }
        }

        mesh->faces = faces;
    }

    bool Parser::isInTriangle(const Vector2 &point, const Vector2 &a, const Vector2 &b, const Vector2 &c) {
        float denominator = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);

        if (denominator == 0)
            return true;
        denominator = 1.0f / denominator;
        float alpha = denominator * ((b.y - c.y) * (point.x - c.x) + (c.x - b.x) * (point.y - c.y));
        float beta = denominator * ((c.y - a.y) * (point.x - c.x) + (a.x - c.x) * (point.y - c.y));
        float gamma = 1.0f - alpha - beta;

        return !(alpha < 0 || beta < 0 || gamma < 0);
    }
}