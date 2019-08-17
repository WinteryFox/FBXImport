#include <chrono>
#include "../include/Decoder.h"

namespace FBX {
    Decoder::Decoder(const std::string &path) : path(path), stream(std::ifstream(path, std::ios::binary)) {}

    Decoder::~Decoder() {
        stream.close();
    }

    Node Decoder::readFile() {
        if (!stream.is_open())
            throw std::invalid_argument("Failed to open file " + path);

        const std::vector<char> FBX_HEADER = {
                'K', 'a', 'y', 'd', 'a', 'r', 'a', ' ', 'F', 'B', 'X', ' ', 'B', 'i', 'n', 'a', 'r', 'y', ' ', ' ',
                '\x00', '\x1a', '\x00'
        };
        if (read(FBX_HEADER.size()) != FBX_HEADER)
            throw std::invalid_argument("Invalid header, file is not an FBX binary file " + path);

        initVersion();

        std::vector<Node> nodes;
        while (stream.good()) {
            Node node;
            if (!readNode(node))
                break;
            nodes.push_back(node);
        }

        Node root;
        root.children = nodes;

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

        std::vector<T> buffer(arraySize);
        std::vector<char> data = read(completeLength);
        if (encoding == 1)
            data = decompress(data);
        assert(data.size() == arraySize * sizeof(T));
        std::memcpy(buffer.data(), data.data(), data.size());

        return buffer;
    }

    std::vector<char> Decoder::decompress(const std::vector<char> &source) const {
        std::vector<char> buffer;
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
            size_t toConsume;
            if (consumed + FBX_CHUNK > source.size())
                toConsume = source.size() - consumed;
            else
                toConsume = FBX_CHUNK;

            infstream.avail_in = toConsume;
            infstream.next_in = (Bytef *) source.data() + consumed;
            consumed += toConsume;

            do {
                infstream.avail_out = FBX_CHUNK;
                infstream.next_out = (Bytef *) out.data();

                ret = inflate(&infstream, Z_NO_FLUSH);

                size_t have = FBX_CHUNK - infstream.avail_out;
                size_t size = buffer.size();
                buffer.resize(buffer.size() + have);
                std::memcpy(buffer.data() + size, out.data(), have);
            } while (infstream.avail_out == 0);
        } while (ret != Z_STREAM_END);
        inflateEnd(&infstream);
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

    bool Decoder::readNode(Node &node) {
        size_t endOffset = readuInt();
        if (endOffset == 0)
            return false;

        node.propertyCount = readuInt();
        node.propertyLength = readuInt();

        node.id = readString();
        node.properties.resize(node.propertyCount);

        for (size_t i = 0; i < node.propertyCount; i++) {
            char dataType = read(1)[0];
            switch (dataType) {
                case 'Y':
                    node.properties[i] = bitcast<uint16_t>(read(2).data()); /// 16 bit int
                    break;
                case 'C':
                    node.properties[i] = bitcast<bool>(read(1).data()); /// 1 bit bool
                    break;
                case 'I':
                    node.properties[i] = bitcast<int32_t>(read(4).data()); /// 32 bit int
                    break;
                case 'F':
                    node.properties[i] = bitcast<float>(read(4).data()); /// 32 bit float
                    break;
                case 'D':
                    node.properties[i] = bitcast<double>(read(8).data()); /// 64 bit float (double)
                    break;
                case 'L':
                    node.properties[i] = bitcast<int64_t>(read(8).data()); /// 64 bit int
                    break;
                case 'R':
                    node.properties[i] = read(readuInt()); /// binary data
                    break;
                case 'S': {
                    auto data = read(readuInt());
                    node.properties[i] = std::string(data.data(), data.size()); /// string
                    break;
                }
                case 'f':
                    node.properties[i] = readArray<float>(); /// array of 32 bit float
                    break;
                case 'i':
                    node.properties[i] = readArray<int32_t>(); /// array of 32 bit int
                    break;
                case 'd':
                    node.properties[i] = readArray<double>(); /// array of 64 bit float (double)
                    break;
                case 'l':
                    node.properties[i] = readArray<int64_t>(); /// array of 64 bit int
                    break;
                case 'b':
                    node.properties[i] = readArray<char>(); /// array of 1 bit boolean
                    break;
                case 'c':
                    node.properties[i] = readArray<char>(); /// array of bytes
                    break;
                default:
                    break;
            }
        }

        if (stream.tellg() < endOffset) {
            while (stream.tellg() < (endOffset - blockLength)) {
                Node child;
                readNode(child);
                node.children.push_back(child);
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