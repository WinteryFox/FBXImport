#pragma once

#include <cassert>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <zlib.h>
#include <any>
#include <optional>
#include "Util.h"
#include "Node.h"

#define FBX_CHUNK 256000

namespace FBX {
    struct Decoder {
        explicit Decoder(const std::string &path);

        ~Decoder();

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

        std::ifstream stream;

        int version = 0;
        size_t blockLength = 0;
        std::vector<char> blockData;
        bool isuInt32 = true;

        std::vector<char> read(size_t length);

        int readuInt();

        std::string readString();

        template<class T>
        std::vector<T> readArray();

        std::vector<char> decompress(const std::vector<char> &source) const;

        bool readNode(Node &node);

        void initVersion();
    };
}