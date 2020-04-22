#pragma once

#include <vector>
#include "Node.h"

namespace FBX {
    static std::vector<Node> findNodes(const Node &node, const std::string &nodeId) {
        std::vector<Node> nodes;

        for (const Node &e : node.children)
            if (e.id == nodeId)
                nodes.push_back(e);
        return nodes;
    }

    template<class T>
    static T getProperty(const Node &properties, const std::string &property, const T &fallback) {
        for (const auto &node : properties.children)
            if (std::get<std::string>(node.properties[0]) == property)
                return std::get<T>(node.properties[4]);
        return fallback;
    }

    template<class T, uint8_t stride>
    static std::vector<T> readLayer(const Node &layer, const std::string &name) {
        std::vector<T> result;

        const auto mappingType = std::get<std::string>(
                findNodes(layer, "MappingInformationType")[0].properties[0]);
        const auto referenceType = std::get<std::string>(
                findNodes(layer, "ReferenceInformationType")[0].properties[0]);
        const auto data = std::get<std::vector<double>>(findNodes(layer, name)[0].properties[0]);

        if (mappingType == "ByPolygonVertex") {
            if (referenceType == "IndexToDirect") {
                const auto indices = std::get<std::vector<int32_t>>(
                        findNodes(layer, name + "Index")[0].properties[0]);

                result.reserve(indices.size());
                for (const auto &index : indices) {
                    if constexpr (stride == 2)
                        result.emplace_back(data[index * stride], data[index * stride + 1]);
                    else if (stride == 3)
                        result.emplace_back(data[index * stride], data[index * stride + 1], data[index * stride + 2]);
                }
            } else if (referenceType == "Direct") {
                for (uint32_t i = 0; i < data.size(); i += stride) {
                    if constexpr (stride == 2)
                        result.emplace_back(data[i], data[i + 1]);
                    else if (stride == 3)
                        result.emplace_back(data[i], data[i + 1], data[i + 2]);
                }
            }
        } else {
            // TODO
        }

        return result;
    }
}