#include "../include/Stream.h"

namespace FBX {
    Stream::Stream(const std::string &path) {
        char *temp = realpath(path.c_str(), nullptr);
        this->path = std::string(temp);
        free(temp);

#ifdef __unix__
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
        // TODO
#endif
    }

    Stream::~Stream() {
        munmap(data, fileSize);
    }

    size_t Stream::tell() const {
        return cursor;
    }
}