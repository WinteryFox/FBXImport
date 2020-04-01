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
    T getProperty(const Node &properties, const std::string &property, const T &fallback) {
        for (const auto &node : properties.children)
            if (std::get<std::string>(node.properties[0]) == property)
                return std::get<T>(node.properties[4]);
        return fallback;
    }
}