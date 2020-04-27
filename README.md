# FBXImport
A simple, easy-to-use and fast C++ library to open and read FBX binary files.

## Installation
Simply add this project to your own, through [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
or any other method. Then add `importer/include` to your included directories and add `FBXImport` to your
linking process.

```CMake
add_subdirectory({CMAKE_CURRENT_SOURCE_DIR}/FBXImport)

add_executable(test sources)
target_link_libraries(test FBXImport)
target_include_directories(test PRIVATE {CMAKE_CURRENT_SOURCE_DIR}/FBXImport/importer/include)
```

## Todo
* Animation support

## Simple example
```C++
const auto &fbxScene = FBX::importFile(path, FBX::Process::TRIANGULATE); // Triangulate all models in the scene.
for (const auto &fbxModel : fbxScene->models) {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec2> uvs;

    for (const auto &v : fbxModel->mesh->vertices)
        vertices.emplace_back(v.x, v.y, v.z);

    for (const auto &face : fbxModel->mesh->faces) {
        if (face.indices.size() != 3) {
            continue; // Skip dots and lines, which cannot be triangulated by the triangulation process.
        }

        indices.push_back(face[0]);
        indices.push_back(face[1]);
        indices.push_back(face[2]);
    }

    for (const auto &uv : fbxModel->mesh->uvs)
        uvs.emplace_back(uv.x, 1.0f - uv.y); // Flip Y-coordinate for usage in Vulkan, if OpenGL or DirectX, don't do this.
}
```
