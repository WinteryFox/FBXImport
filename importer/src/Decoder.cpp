#include "../include/FBX/Decoder.h"

namespace FBX {
    using namespace std::string_view_literals;

    Decoder::Decoder(const std::string &path) : path(path), stream(path) {}

    Node Decoder::readFile() {
        auto FBX_HEADER = "Kaydara FBX Binary  \x00\x1a\x00"sv;
        auto temp = stream.read<char>(FBX_HEADER.size());
        if (FBX_HEADER != std::string_view(temp.begin, FBX_HEADER.size()))
            throw std::invalid_argument("Invalid header, file is not an FBX binary file " + path);

        initVersion();

        std::vector<Node> nodes;
        while (true) {
            Node node;
            if (!readNode(node))
                break;
            nodes.push_back(node);
        }

        Node root;
        root.children = nodes;

        return root;
    }

    int Decoder::readuInt() {
        if (isuInt32)
            return stream.read<uint32_t>();
        else
            return stream.read<uint64_t>();
    }

    std::string Decoder::readString() {
        size_t size = stream.read<uint8_t>();
        auto data = stream.read<char>(size);
        return std::string(data.begin, data.end);
    }

    template<class T>
    std::vector<T> Decoder::readArray() {
        size_t arraySize = stream.read<std::uint32_t>();
        size_t encoding = stream.read<std::uint32_t>();
        size_t completeLength = stream.read<std::uint32_t>();

        std::vector<T> buffer;
        if (encoding == 1)
            buffer = decompress<T>(stream.read<char>(completeLength));
        else {
            auto data = stream.read<T>(completeLength);
            buffer.resize(arraySize);
            std::memcpy(buffer.data(), data.begin, data.size() * sizeof(T));
        }

        return buffer;
    }

    template<class T>
    std::vector<T> Decoder::decompress(const Span<char> &source) const {
        std::vector<T> buffer;
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
            infstream.next_in = (Bytef *) source.begin + consumed;
            consumed += toConsume;

            do {
                infstream.avail_out = FBX_CHUNK;
                infstream.next_out = (Bytef *) out.data();

                ret = inflate(&infstream, Z_NO_FLUSH);

                size_t have = FBX_CHUNK - infstream.avail_out;
                size_t size = buffer.size() * sizeof(T);

                assert(toConsume <= have); // TODO: Fix buffer overflow for large chunks to inflate

                buffer.resize(buffer.size() + have / sizeof(T));
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
            auto dataType = stream.read<char>();
            switch (dataType) {
                case 'Y':
                    node.properties[i] = stream.read<int16_t>(); /// 16 bit int
                    break;
                case 'C':
                    node.properties[i] = stream.read<bool>(); /// 1 bit bool
                    break;
                case 'I':
                    node.properties[i] = stream.read<int32_t>(); /// 32 bit int
                    break;
                case 'F':
                    node.properties[i] = stream.read<float>(); /// 32 bit float
                    break;
                case 'D':
                    node.properties[i] = stream.read<double>(); /// 64 bit float (double)
                    break;
                case 'L':
                    node.properties[i] = stream.read<int64_t>(); /// 64 bit int
                    break;
                case 'R': {
                    const auto &data = stream.read<char>(readuInt());
                    node.properties[i] = std::vector<char>(data.begin, data.end); /// binary data
                    break;
                }
                case 'S': {
                    const auto &data = stream.read<char>(readuInt());
                    node.properties[i] = std::string(data.begin, data.end); /// string
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

        if (stream.tell() < endOffset) {
            while (stream.tell() < (endOffset - blockLength)) {
                Node child;
                readNode(child);
                node.children.push_back(child);
            }

            if (stream.read<char>(blockLength) != Span<char>(blockData.data(), blockData.size()))
                throw std::runtime_error("Failed to read nested block");
        }

        if (stream.tell() < endOffset)
            throw std::runtime_error("Scope length not reached");

        return true;
    }
}