#include "../include/FBX/Parser.h"

namespace FBX {
    Parser::~Parser() {
        for (auto &process : processes)
            delete process;
    }

    std::unique_ptr<const Scene> Parser::parseScene() const {
        Settings settings(findNodes(root, "GlobalSettings")[0]);

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
            if (std::get<std::string>(node.properties[2]) == "Mesh")
                meshes.push_back(parseMesh(node, {})); // TODO

        std::vector<std::shared_ptr<Model>> models;
        models.reserve(nodeModels.size());
        for (const auto &node : nodeModels)
            if (std::get<std::string>(node.properties[2]) == "Mesh")
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

        return std::make_unique<Scene>(models, settings);
    }

    std::shared_ptr<Mesh> Parser::parseMesh(const Node &node, const Vector3 up) const {
        std::shared_ptr<Mesh> mesh(new Mesh(node));

        for (const auto &process : processes)
            process->execute(mesh);

        for (auto &vertex : mesh->vertices) {
            Vector3 v = vertex;

            // TODO
            /*if (processes & MAKE_X_UP) {
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
            }*/

            vertex = v;
        }

        return mesh;
    }
}