#include "../include/Decoder.h"

namespace FBX {
    const std::vector<char> FBX_HEADER = {
            'K', 'a', 'y', 'd', 'a', 'r', 'a', ' ', 'F', 'B', 'X', ' ', 'B', 'i', 'n', 'a', 'r', 'y', ' ', ' ', '\x00',
            '\x1a', '\x00'
    };

    Decoder::Decoder(const std::string &path) : stream(std::ifstream(path, std::ios::binary)) {
        if (!stream.is_open()) {
            std::cerr << "Failed to open file " << path << std::endl;
            return;
        }

        if (read(FBX_HEADER.size()) != FBX_HEADER) {
            std::cerr << "File has incorrect header" << std::endl;
            return;
        }

        initVersion();
    }

    Decoder::~Decoder() {
        stream.close();
    }

    Element Decoder::readFile() {
        std::vector<Element> elements;
        while (stream.good()) {
            Element element;
            if (!readElement(element))
                break;
            elements.push_back(element);
        }
        Element root;
        root.children = elements;
        return root;
    }

    template<class T>
    T Decoder::bitcast(char* data) {
        T result;
        std::memcpy(&result, data, sizeof(T));
        return result;
    }

    std::vector<char> Decoder::read(size_t length) {
        std::vector<char> buffer(length, '\x00');
        stream.read(buffer.data(), length);
        return buffer;
    }

    int Decoder::readuInt() {
        std::vector<char> buffer;
        if (isuInt32)
            buffer = read(4);
        else
            buffer = read(8);

        return bitcast<int>(buffer.data());
    }

    std::string Decoder::readString() {
        int size = read(1)[0];
        return std::string(read(size).data(), size);
    }

    template<class T>
    std::vector<T> Decoder::readArray(DataType type) {
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
            infstream.avail_in = temp.size();
            infstream.next_in = (Bytef*) temp.data();
            infstream.avail_out = buffer.size();
            infstream.next_out = (Bytef*) buffer.data();

            inflateInit(&infstream);
            inflate(&infstream, Z_NO_FLUSH);
            inflateEnd(&infstream);
        } else {
            buffer = std::move(temp);
        }

        std::vector<T> result;
        result.reserve(buffer.size() / (int) type);
        for (size_t i = 0; i < buffer.size(); i += (int) type) {
            result.push_back(bitcast<T>(buffer.data() + i));
        }

        return result;
    }

    void Decoder::initVersion() {
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

    bool Decoder::readElement(Element &element) {
        size_t endOffset = readuInt();
        if (endOffset == 0)
            return false;

        element.propertyCount = readuInt();
        element.propertyLength = readuInt();

        element.id = readString();
        element.data.resize(element.propertyCount);

        for (size_t i = 0; i < element.propertyCount; i++) {
            char dataType = read(1)[0];
            switch (dataType) {
                case 'Y':
                    element.data[i] = bitcast<short>(read(2).data());
                    break;
                case 'C':
                    element.data[i] = bitcast<bool>(read(1).data());
                    break;
                case 'I':
                    element.data[i] = bitcast<int>(read(4).data());
                    break;
                case 'F':
                    element.data[i] = bitcast<float>(read(4).data());
                    break;
                case 'D':
                    element.data[i] = bitcast<double>(read(8).data());
                    break;
                case 'L':
                    element.data[i] = bitcast<long long>(read(8).data());
                    break;
                case 'R':
                    element.data[i] = read(readuInt());
                    break;
                case 'S':
                    element.data[i] = read(readuInt());
                    break;
                case 'f':
                    element.data[i] = readArray<float>(DataType::ARRAY_FLOAT32);
                    break;
                case 'i':
                    element.data[i] = readArray<int>(DataType::ARRAY_INT32);
                    break;
                case 'd':
                    element.data[i] = readArray<float>(DataType::ARRAY_FLOAT64);
                    break;
                case 'l':
                    element.data[i] = readArray<int>(DataType::ARRAY_INT64);
                    break;
                case 'b':
                    element.data[i] = readArray<bool>(DataType::ARRAY_BOOL);
                    break;
                case 'c':
                    element.data[i] = readArray<char>(DataType::ARRAY_BYTE);
                    break;
                default:
                    break;
            }
        }

        if (stream.tellg() < endOffset) {
            while (stream.tellg() < (endOffset - blockLength)) {
                Element child;
                readElement(child);
                element.children.push_back(child);
            }

            if (read(blockLength) != blockData) {
                throw std::runtime_error("Failed to read nested block");
            }
        }

        if (stream.tellg() < endOffset)
            throw std::runtime_error("Scope length not reached");

        return true;
    }
}