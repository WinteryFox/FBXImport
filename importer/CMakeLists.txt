find_package(ZLIB REQUIRED)

add_library(FBXImport SHARED include/Decoder.h src/Decoder.cpp include/Node.h include/Parser.h include/Mesh.h include/Vector3.h src/Parser.cpp include/FBXImport.h include/Span.h include/Stream.h src/Stream.cpp)
target_include_directories(FBXImport PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(FBXImport ZLIB::ZLIB)