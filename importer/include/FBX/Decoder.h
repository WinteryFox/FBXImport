#pragma once

#include <cassert>
#include <string>
#include <iostream>
#include <cstring>
#include <zlib.h>
#include <variant>
#include <optional>
#include "Node.h"
#include "Stream.h"

#define FBX_CHUNK 256000

namespace FBX {
    struct Decoder {
        explicit Decoder(const std::string &path);

        /**
         * Reads the entire file and returns the root
         *
         * @param[out] root The root node of the file
         * @return Returns OK on success, INVALID_FILE on failure to open the file and INVALID_HEADER when the header
         * check failed indicating that the file is not an FBX binary file
         */
        Node readFile();

    private:
        const std::string path;

        Stream stream;

        int version = 0;
        size_t blockLength = 0;
        std::vector<char> blockData;
        bool isuInt32 = true;

        int readuInt();

        std::string readString();

        template<class T>
        std::vector<T> readArray();

        template<class T>
        [[nodiscard]] std::vector<T> decompress(const Span<char> &source) const;

        bool readNode(Node &node);

        void initVersion();
    };
}