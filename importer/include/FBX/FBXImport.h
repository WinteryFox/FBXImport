#include "Decoder.h"
#include "Parser.h"
#include "Mesh.h"
#include "Node.h"
#include "Vector3.h"

namespace FBX {
    static std::unique_ptr<const Scene> importFile(const std::string &path, int processes) {
        Decoder decoder(path);
        auto root = decoder.readFile();

        Parser parser(root, processes);
        return parser.parseScene();
    }
}