#pragma once

#include <cstdint>

namespace FBX {
    struct Object {
        int64_t id;

        explicit Object(int64_t id) : id(id) {}
    };
}