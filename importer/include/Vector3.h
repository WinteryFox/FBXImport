#pragma once

namespace FBX {
    struct Vector3 {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        Vector3() = default;
        explicit Vector3(float initializer) : x(initializer), y(initializer), z(initializer) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };
}