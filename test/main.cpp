#include <Parser.h>

int main() {
    auto parser = FBX::Parser("new-fox.fbx");
    parser.readFile();

    return EXIT_SUCCESS;
}