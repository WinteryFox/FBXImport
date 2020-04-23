#pragma once

#include "Texture.h"

namespace FBX {
    struct Material : public Object {
        std::shared_ptr<Texture> texture = nullptr;
        const std::string shadingModel;
        const double emissiveFactor;
        const Vector3 ambientColor;
        const Vector3 diffuseColor;
        const Vector3 transparentColor;
        const Vector3 specularColor;
        const double shininessExponent;
        const Vector3 emissive;
        const Vector3 ambient;
        const Vector3 diffuse;
        const Vector3 specular;
        const double shininess;
        const double opacity;
        const double reflectivity;

        explicit Material(const Node &node) :
                Object(std::get<int64_t>(node.properties[0])),
                shadingModel(getProperty<std::string>(node, "ShadingModel", "phong")),
                emissiveFactor(getProperty(node, "EmissiveFactor", 0.0)),
                ambientColor(getProperty(node, "AmbientColor", {})),
                diffuseColor(getProperty(node, "DiffuseColor", {})),
                transparentColor(getProperty(node, "TransparentColor", {})),
                specularColor(getProperty(node, "SpecularColor", {})),
                shininessExponent(getProperty(node, "ShininessExponent", 2.0)),
                emissive(getProperty(node, "Emissive", {})),
                ambient(getProperty(node, "Ambient", {})),
                diffuse(getProperty(node, "Diffuse", {})),
                specular(getProperty(node, "Specular", {})),
                shininess(getProperty(node, "Shininess", 2.0)),
                opacity(getProperty(node, "Opacity", 1.0)),
                reflectivity(getProperty(node, "Reflectivity", 0.0)) {}
    };
}