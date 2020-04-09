#pragma once

namespace FBX {
    struct Vector2 {
        float x = 0.0;
        float y = 0.0;

        Vector2() = default;

        Vector2(float x, float y) : x(x), y(y) {}

        explicit Vector2(float init) : x(init), y(init) {}
    };
}