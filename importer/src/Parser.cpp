#include <any>
#include "../include/Parser.h"

namespace FBX {
    const std::vector<char> FBX_HEADER = {
            'K', 'a', 'y', 'd', 'a', 'r', 'a', ' ', 'F', 'B', 'X', ' ', 'B', 'i', 'n', 'a', 'r', 'y', ' ', ' ', '\x00',
            '\x1a', '\x00'
    };

    //const std::string_view FBX_HEADER = "Kaydara FBX Binary  \x00\x1a\x00"sv;

    Parser::Parser(const std::string &path) : stream(std::ifstream(path, std::ios::binary)) {
        if (!stream.is_open()) {
            std::cerr << "Failed to open file " << path << std::endl;
            return;
        }
    }

    void Parser::readFile() {
        if (read(FBX_HEADER.size()) != FBX_HEADER) {
            std::cerr << "File has incorrect header" << std::endl;
            return;
        }

        initVersion();

        std::vector<Element> elements;
        while (stream.good()) {
            elements.push_back(readElement());
        }
        std::cout << "yay" << std::endl;
    }

    template<class T>
    T Parser::bitcast(char* data) {
        T result;
        std::memcpy(&result, data, sizeof(T));
        return result;
    }

    std::vector<char> Parser::read(size_t length) {
        std::vector<char> buffer(length, '\x00');
        stream.read(buffer.data(), length);
        return buffer;
    }

    int Parser::readuInt() {
        std::vector<char> buffer;
        if (isuInt32)
            buffer = read(4);
        else
            buffer = read(8);

        return bitcast<int>(buffer.data());
    }

    std::string Parser::readString() {
        int size = read(1)[0];
        return std::string(read(size).data(), size);
    }

    template<class T>
    std::vector<T> Parser::readArray(DataType type) {
        int length = readuInt();
        int encoding = readuInt();
        int compLength = readuInt();

        std::vector<char> temp = read(compLength);
        std::vector<char> buffer(length);
        if (encoding == 1) {
            z_stream infstream;
            infstream.zalloc = Z_NULL;
            infstream.zfree = Z_NULL;
            infstream.opaque = Z_NULL;
            infstream.avail_in = compLength;
            infstream.next_in = (Bytef*) temp.data();
            infstream.avail_out = length;
            infstream.next_out = (Bytef*) buffer.data();

            inflateInit(&infstream);
            inflate(&infstream, Z_NO_FLUSH);
            inflateEnd(&infstream);
        } else {
            buffer = std::vector(temp);
        }

        std::vector<T> result(length / (int) type);
        for (size_t i = 0; i < length; i += (int) type) {
            result.push_back(bitcast<T>(buffer.data() + i));
        }

        return result;
    }

    void Parser::initVersion() {
        version = readuInt();

        if (version < 7500) {
            blockLength = 13;
            isuInt32 = true;
        } else {
            blockLength = 25;
            isuInt32 = false;
        }
        blockData = std::vector(blockLength, '\0');
    }

    Element Parser::readElement() {
        size_t endOffset = readuInt();
        if (endOffset == 0)
            return Element();

        Element element;

        element.propertyCount = readuInt();
        element.propertyLength = readuInt();

        element.elementId = readString();
        std::vector<char> elementPropertiesType(element.propertyCount);
        std::vector<std::any> elementPropertiesData(element.propertyCount);

        for (size_t i = 0; i < element.propertyCount; i++) {
            char dataType = read(1)[0];
            switch (dataType) {
                case 'Y':
                    elementPropertiesData[i] = bitcast<short>(read(2).data());
                    break;
                case 'C':
                    elementPropertiesData[i] = bitcast<bool>(read(1).data());
                    break;
                case 'I':
                    elementPropertiesData[i] = bitcast<int>(read(4).data());
                    break;
                case 'F':
                    elementPropertiesData[i] = bitcast<float>(read(4).data());
                    break;
                case 'D':
                    elementPropertiesData[i] = bitcast<double>(read(8).data());
                    break;
                case 'L':
                    elementPropertiesData[i] = bitcast<long long>(read(8).data());
                    break;
                case 'R':
                    elementPropertiesData[i] = read(readuInt());
                    break;
                case 'S':
                    elementPropertiesData[i] = read(readuInt());
                    break;
                case 'f':
                    elementPropertiesData[i] = readArray<float>(DataType::ARRAY_FLOAT32);
                    break;
                case 'i':
                    elementPropertiesData[i] = readArray<int>(DataType::ARRAY_INT32);
                    break;
                case 'd':
                    elementPropertiesData[i] = readArray<float>(DataType::ARRAY_FLOAT64);
                    break;
                case 'l':
                    elementPropertiesData[i] = readArray<int>(DataType::ARRAY_INT64);
                    break;
                case 'b':
                    elementPropertiesData[i] = readArray<bool>(DataType::ARRAY_BOOL);
                    break;
                case 'c':
                    elementPropertiesData[i] = readArray<char>(DataType::ARRAY_BYTE);
                    break;
                default:
                    break;
            }
        }

        if (stream.tellg() < endOffset) {
            while (stream.tellg() < (endOffset - blockLength)) {
                element.children.push_back(readElement());
            }

            if (read(blockLength) != blockData) {
                throw std::runtime_error("Failed to read nested block");
            }
        }

        if (stream.tellg() < endOffset)
            throw std::runtime_error("Scope length not reached");

        return element;
    }
}