#pragma once

#include <cassert>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <zlib.h>
#include <any>
#include "Util.h"
#include "Element.h"

#define FBX_CHUNK 256000

namespace FBX {
    struct Decoder {
        explicit Decoder(const std::string &path);

        ~Decoder();

        Element readFile();

        int version;

    private:
        std::ifstream stream;

        size_t blockLength;
        std::vector<char> blockData;
        bool isuInt32 = true;

        std::vector<char> read(size_t length);

        int readuInt();

        std::string readString();

        template<class T>
        std::vector<T> readArray();

        bool readElement(Element &element);

        void initVersion();
    };
}