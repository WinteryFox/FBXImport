#pragma once

#include "Vector3.h"

namespace FBX {
    struct Settings {
        Vector3 upAxis;
        Vector3 frontAxis;
        Vector3 coordAxis;
        Vector3 originalUpAxis;
        double unitScaleFactor;
        double originalUnitScaleFactor;
        Vector3 ambientColor;

        explicit Settings(const Node &node)
                : upAxis(parseAxis(getProperty<int32_t>(node, "UpAxis", 2),
                                   getProperty<int32_t>(node, "UpAxisSign", 1))),
                  frontAxis(parseAxis(getProperty<int32_t>(node, "FrontAxis", 2),
                                      getProperty<int32_t>(node, "FrontAxisSign", 1))),
                  coordAxis(parseAxis(getProperty<int32_t>(node, "CoordAxis", 2),
                                      getProperty<int32_t>(node, "CoordAxisSign", 1))),
                  originalUpAxis(parseAxis(getProperty<int32_t>(node, "OriginalUpAxis", 2),
                                           getProperty<int32_t>(node, "OriginalUpAxisSign", 1))),
                  unitScaleFactor(getProperty<double>(node, "UnitScaleFactor", 1.0)),
                  originalUnitScaleFactor(getProperty<double>(node, "OriginalUnitScaleFactor", 1.0)),
                  ambientColor(getProperty(node, "AmbientColor", {})) {}
    };
}