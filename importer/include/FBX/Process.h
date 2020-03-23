#pragma once

namespace FBX {
    enum Process {
        TRIANGULATE = 0x001,
        GENERATE_UVS = 0x002,
        MAKE_X_UP = 0x004,
        MAKE_Y_UP = 0x008,
        MAKE_Z_UP = 0x0010,
    };
}