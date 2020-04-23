#include <FBX/FBXImport.h>
#include <chrono>

int main(int argc, char *argv[]) {
    auto t1 = std::chrono::high_resolution_clock::now();

    const auto &result = FBX::importFile("Fox.fbx", FBX::Process::TRIANGULATE | FBX::Process::MAKE_Y_UP);

    for (const auto &face : result->models[0]->mesh->faces)
        if (face.indices.size() != 3)
            throw std::runtime_error(std::to_string(face.indices.size()));

    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
              << " milliseconds to read file" << std::endl;

    // Use the mesh

    return EXIT_SUCCESS;
}