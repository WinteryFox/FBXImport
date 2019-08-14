#include <Decoder.h>

int main() {
    auto decoder = FBX::Decoder("new-fox.fbx");
    auto element = decoder.readFile();

    return EXIT_SUCCESS;
}