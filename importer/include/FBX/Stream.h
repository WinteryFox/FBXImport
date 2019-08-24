#pragma once

#ifdef __unix__

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#elif __WIN32__

#include <windows.h>

#endif

#include <string>
#include <stdexcept>
#include <cassert>
#include "Span.h"

namespace FBX {
    class Stream {
        std::string path;
        char *data;
        size_t cursor = 0;

#ifdef __WIN32__
        HANDLE file;
        HANDLE map;
#elif __unix__
        size_t fileSize;
#endif

    public:
        explicit Stream(const std::string &path);

        ~Stream();

        [[nodiscard]] size_t tell() const;

        template<class T>
        T read() {
            auto value = reinterpret_cast<T*>(data + cursor);
            cursor += sizeof(T);
            return *value;
        }

        template<class T>
        Span<T> read(size_t length) {
            auto span = Span<T>(reinterpret_cast<T*>(data + cursor), length);
            cursor += length;
            return span;
        }
    };
}