# FBXImport
A simple, easy-to-use and fast C++ library to open and read FBX binary files.

## Installation
Simply add this project to your own, through [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
or by installing the project on your machine and using the [provided find_package script](https://github.com/WinteryFox/FBXImport/blob/master/cmake/FindFBXImport.cmake).
Then add `importer/include` to your included directories and add `FBXImport` to your
linking process and include `FBX/FBXImport.h`.

```CMake
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/FBXImport)

add_executable(test main.cpp)
target_link_libraries(test FBXImport)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FBXImport/importer/include)
```

## Todo
* Animation support

## Importing models
Loading a model from a file is as easy as including `FBX/FBXImport.h` and calling `FBX::importFile`.
```C++
#include <FBX/FBXImport.h>
#include <FBX/TriangulateProcess.h>

const auto &result = FBX::importFile(path, std::set<FBX::Process*>{new FBX::TriangulateProcess()});
```

## Example using GLM
```C++
#include <FBX/FBXImport.h>
#include <FBX/TriangulateProcess.h>
#include <glm/glm.hpp>

const auto &result = FBX::importFile(path, std::set<FBX::Process*>{new FBX::TriangulateProcess()}); // Triangulate all models in the scene.
for (const auto &fbxModel : fbxScene->models) {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec2> uvs;

    vertices.reserve(fbxModel->mesh->vertices.size());
    for (const auto &v : fbxModel->mesh->vertices)
        vertices.emplace_back(v.x, v.y, v.z);

    indices.reserve(fbxModel->mesh->indexCount);
    for (const auto &face : fbxModel->mesh->faces) {
        if (face.indices.size() != 3) {
            continue; // Skip dots and lines, which cannot be triangulated by the triangulation process.
        }

        indices.push_back(face[0]);
        indices.push_back(face[1]);
        indices.push_back(face[2]);
    }

    uvs.reserve(fbxModel->mesh->uvs.size());
    for (const auto &uv : fbxModel->mesh->uvs)
        uvs.emplace_back(uv.x, 1.0f - uv.y); // Flip Y-coordinate for usage in Vulkan.
}
```

## Creating your own processes
You can create and apply your own processes by extending from `FBX/Process.h` and adding it to the set of processes in your `FBX::importFile` call.
For example, after creating a process called `MyProcess` you would call  
```C++
FBX::importFile(path, std::set<Process*>{new MyProcess()});
```
which will apply the process `MyProcess` on every mesh in the file.
