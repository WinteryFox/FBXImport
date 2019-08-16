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
    std::vector<T> Decoder::readArray() {
        size_t arraySize = readuInt();
        size_t encoding = readuInt();
        size_t completeLength = readuInt();

        std::vector<T> buffer;
        if (encoding == 1) {
            std::vector<char> in(FBX_CHUNK);
            std::vector<char> out(FBX_CHUNK);

            z_stream infstream;
            infstream.zalloc = Z_NULL;
            infstream.zfree = Z_NULL;
            infstream.opaque = Z_NULL;
            infstream.avail_in = 0;
            infstream.next_in = Z_NULL;

            if (inflateInit(&infstream) != Z_OK) {
                throw std::runtime_error("Failed to initialize inflate");
            }

            int ret;
            size_t consumed = 0;
            do {
                size_t toConsume = 0;
                if (consumed + FBX_CHUNK > completeLength)
                    toConsume = completeLength - consumed;
                else
                    toConsume = FBX_CHUNK;

                in = read(toConsume);

                infstream.avail_in = toConsume;
                infstream.next_in = (Bytef*) in.data();

                do {
                    infstream.avail_out = FBX_CHUNK;
                    infstream.next_out = (Bytef*) out.data();

                    ret = inflate(&infstream, Z_NO_FLUSH);

                    size_t have = FBX_CHUNK - infstream.avail_out;
                    assert(have % sizeof(T) == 0);
                    for (size_t i = 0; i < arraySize; i++)
                        buffer.push_back(bitcast<T>(out.data() + i * sizeof(T)));
                } while (infstream.avail_out == 0);
            } while (ret != Z_STREAM_END);
            inflateEnd(&infstream);
            assert(buffer.size() == arraySize);
        } else {
            std::vector<char> in = read(completeLength);

            assert(in.size() % sizeof(T) == 0);
            for (size_t i = 0; i < arraySize; i++)
                buffer.push_back(bitcast<T>(in.data() + i * sizeof(T)));
            assert(buffer.size() == arraySize);
        }
        return buffer;
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
        element.properties.resize(element.propertyCount);

        for (size_t i = 0; i < element.propertyCount; i++) {
            char dataType = read(1)[0];
            switch (dataType) {
                case 'Y':
                    element.properties[i] = bitcast<uint16_t>(read(2).data());
                    break;
                case 'C':
                    element.properties[i] = bitcast<bool>(read(1).data());
                    break;
                case 'I':
                    element.properties[i] = bitcast<int32_t>(read(4).data());
                    break;
                case 'F':
                    element.properties[i] = bitcast<float>(read(4).data());
                    break;
                case 'D':
                    element.properties[i] = bitcast<double>(read(8).data());
                    break;
                case 'L':
                    element.properties[i] = bitcast<int64_t>(read(8).data());
                    break;
                case 'R':
                    element.properties[i] = read(readuInt());
                    break;
                case 'S': {
                    auto data = read(readuInt());
                    element.properties[i] = std::string(data.begin(), data.end());
                    break;
                }
                case 'f':
                    element.properties[i] = readArray<float>();
                    break;
                case 'i':
                    element.properties[i] = readArray<int32_t>();
                    break;
                case 'd':
                    element.properties[i] = readArray<float>();
                    break;
                case 'l':
                    element.properties[i] = readArray<int64_t>();
                    break;
                case 'b':
                    element.properties[i] = readArray<bool>();
                    break;
                case 'c':
                    element.properties[i] = readArray<char>();
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