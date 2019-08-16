#include <Decoder.h>
#include <Parser.h>

int main() {
    auto decoder = FBX::Decoder("new-fox.fbx");
    auto element = decoder.readFile();

    FBX::parseMesh(element);

    return EXIT_SUCCESS;
}