#include <array>
#include <memory>
#include "../include/FBX/Parser.h"
#include "../include/FBX/Model.h"

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
            // TODO: currently only supports quads to triangles
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

    void Parser::triangulate(const std::shared_ptr<Mesh> &mesh) {
        std::vector<Face> faces;

        for (const auto &face : mesh->faces) {
            /// Is this face a quad
            if (face.indices.size() == 4) {
                Face face1;
                Face face2;
                face1.indices.reserve(3);
                face2.indices.reserve(3);

                uint32_t startVertex = 0;
                for (uint32_t i = 0; i < 4; i++) {
                    const auto &v0 = mesh->vertices[face[(i + 3) % 4]];
                    const auto &v1 = mesh->vertices[face[(i + 2) % 4]];
                    const auto &v2 = mesh->vertices[face[(i + 1) % 4]];
                    const auto &v = mesh->vertices[face[i]];

                    const auto left = v0 - v;
                    const auto diag = v1 - v;
                    const auto right = v2 - v;

                    const float angle = std::acos((left * diag).x) + std::acos((right * diag).x);
                    if (angle > M_PI) {
                        startVertex = i;
                        break;
                    }
                }

                face1.indices.push_back(face[startVertex]);
                face1.indices.push_back(face[(startVertex + 1) % 4]);
                face1.indices.push_back(face[(startVertex + 2) % 4]);

                face2.indices.push_back(face[startVertex]);
                face2.indices.push_back(face[(startVertex + 2) % 4]);
                face2.indices.push_back(face[(startVertex + 3) % 4]);

                faces.push_back(face1);
                faces.push_back(face2);
            } else {
                // TODO
            }
        }

        mesh->faces = faces;
    }
}