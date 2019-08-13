find_package(ZLIB REQUIRED)

add_library(FBXImport STATIC include/Parser.h src/Parser.cpp include/Element.h)
target_include_directories(FBXImport PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(FBXImport -static ZLIB::ZLIB)