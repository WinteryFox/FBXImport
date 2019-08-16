#include <Decoder.h>
#include <Parser.h>

int main() {
    FBX::Decoder decoder("new-fox.fbx");
    auto root = decoder.readFile();

    FBX::Parser parser(root);
    auto mesh = parser.parseMesh();

    return EXIT_SUCCESS;
}