#include "Decoder.h"
#include "Parser.h"
#include "Mesh.h"
#include "Node.h"
#include "Util.h"

namespace FBX {
    static Mesh importFile(const std::string &path) {
        Decoder decoder(path);
        auto root = decoder.readFile();

        Parser parser(root);
        return parser.parseMesh();
    }
}