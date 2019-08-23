#include "../include/FBX/Stream.h"

namespace FBX {
    Stream::Stream(const std::string &path) {
#ifdef __unix__
        char *temp = realpath(path.c_str(), nullptr);
        this->path = std::string(temp);
        free(temp);

        struct stat st{};
        stat(this->path.c_str(), &st);
        fileSize = st.st_size;
        int file = open(this->path.c_str(), O_RDONLY, 0);
        if (file == -1)
            throw std::runtime_error("Failed to open file " + path);

        data = static_cast<char *>(mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, file, 0));
        if (data == MAP_FAILED)
            throw std::runtime_error("Failed to map file " + path);
#elif __WIN32__
        auto buffer = new char[512];
        GetFullPathName(path.c_str(), 512, buffer, nullptr);
        this->path = reinterpret_cast<const char *>(buffer);

        file = CreateFile(
                this->path.c_str(),
                GENERIC_READ,
                0,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr);
        if (!file) {
            throw std::runtime_error("Failed to open file " + path);
        }

        map = CreateFileMapping(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (!map) {
            CloseHandle(file);
            throw std::runtime_error("Failed to create file mapping " + path);
        }

        data = (char *) MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
        if (!data) {
            CloseHandle(map);
            CloseHandle(file);
            throw std::runtime_error("Failed to memory map file " + path);
        }
#endif
    }

    Stream::~Stream() {
#ifdef __unix__
        munmap(data, fileSize);
#elif __WIN32__
        UnmapViewOfFile(data);
        CloseHandle(map);
        CloseHandle(file);
#endif
    }

    size_t Stream::tell() const {
        return cursor;
    }
}