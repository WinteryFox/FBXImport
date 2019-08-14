#include <Decoder.h>

int main() {
    auto decoder = FBX::Decoder("new-fox.fbx");
    FBX::Element element = decoder.readFile();

    return EXIT_SUCCESS;
}