#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <zlib.h>
#include <any>
#include "../include/Element.h"

namespace FBX {
    enum class DataType {
        ARRAY_FLOAT32 = 4,
        ARRAY_INT32 = 4,
        ARRAY_FLOAT64 = 8,
        ARRAY_INT64 = 8,
        ARRAY_BOOL = 1,
        ARRAY_BYTE = 1
    };

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
        std::vector<T> readArray(DataType type);

        template<class T>
        T bitcast(char* data);

        bool readElement(Element &element);

        void initVersion();
    };
}