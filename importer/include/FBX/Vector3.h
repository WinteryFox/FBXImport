#pragma once

#include <stdexcept>
#include <cmath>

namespace FBX {
    struct Vector3 {
        float x = 0;
        float y = 0;
        float z = 0;

        Vector3() = default;

        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        explicit Vector3(float init) : x(init), y(init), z(init) {}

        float &operator[](int index) {
            if (index == 0)
                return x;
            else if (index == 1)
                return y;
            else if (index == 2)
                return z;
            else
                throw std::runtime_error("Access out of bounds");
        }

        bool operator==(const Vector3 &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        Vector3 operator-(const Vector3 &other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        void operator-=(const Vector3 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
        }

        float operator*(const Vector3 &other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        void normalize() {
            const float length = std::sqrt((x * x) + (y * y) + (z * z));
            x /= length;
            y /= length;
            z /= length;
        }
    };
}