#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include "Process.h"

namespace FBX {
    struct TriangulateProcess : public Process {
        void execute(const std::shared_ptr<Mesh> &mesh) override;

    private:
        static bool isInTriangle(const Vector2 &point, const Vector2 &a, const Vector2 &b, const Vector2 &c);

        static bool onLeftSideOfLine(const Vector2 &a, const Vector2 &b, const Vector2 &c);
    };
}