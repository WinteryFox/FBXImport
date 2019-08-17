find_package(ZLIB REQUIRED)

add_library(FBXImport SHARED include/Decoder.h src/Decoder.cpp include/Node.h include/Parser.h include/Mesh.h include/Vector3.h include/Util.h src/Parser.cpp include/FBXImport.h include/Span.h)
target_include_directories(FBXImport PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(FBXImport ZLIB::ZLIB)