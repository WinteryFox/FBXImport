#include "Decoder.h"
#include "Parser.h"
#include "Mesh.h"
#include "Node.h"

namespace FBX {
    static Scene importFile(const std::string &path, int processes) {
        Decoder decoder(path);
        auto root = decoder.readFile();

        Parser parser(root, processes);
        return parser.parseScene();
    }
}