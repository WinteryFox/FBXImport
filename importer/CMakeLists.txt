find_package(ZLIB REQUIRED)

add_library(FBXImport STATIC include/Decoder.h src/Decoder.cpp include/Element.h include/Parser.h include/Mesh.h include/Vector3.h include/Util.h)
target_include_directories(FBXImport PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(FBXImport -static ZLIB::ZLIB)