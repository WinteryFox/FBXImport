#pragma once

namespace FBX {
    struct Vector3 {
        float x = 0;
        float y = 0;
        float z = 0;

        Vector3() = default;

        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        explicit Vector3(float init) : x(init), y(init), z(init) {}
    };
}