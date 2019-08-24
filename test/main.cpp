#include <FBX/FBXImport.h>
#include <chrono>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Test app only takes 1 input argument, the path to the file." << std::endl;
        return EXIT_FAILURE;
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    auto result = FBX::importFile(argv[1], FBX::Process::TRIANGULATE);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
              << " milliseconds to read file" << std::endl;

    // Use the mesh

    return EXIT_SUCCESS;
}