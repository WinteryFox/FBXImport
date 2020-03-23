#include <FBX/FBXImport.h>
#include <chrono>

int main(int argc, char *argv[]) {
    auto t1 = std::chrono::high_resolution_clock::now();

    auto result = FBX::importFile("fox.fbx", FBX::Process::TRIANGULATE | FBX::Process::MAKE_Y_UP);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
              << " milliseconds to read file" << std::endl;

    // Use the mesh

    return EXIT_SUCCESS;
}