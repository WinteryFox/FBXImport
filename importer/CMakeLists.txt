find_package(ZLIB REQUIRED)

set(fbx_SOURCES
        src/Decoder.cpp
        src/Parser.cpp
        src/Stream.cpp)
set(fbx_HEADERS
        include/Decoder.h
        include/Node.h
        include/Parser.h
        include/Mesh.h
        include/FBXImport.h
        include/Span.h
        include/Stream.h
        include/Scene.h)

add_library(FBXImport SHARED ${fbx_SOURCES})
target_include_directories(FBXImport PUBLIC ${ZLIB_INCLUDE_DIRS})
target_link_libraries(FBXImport ZLIB::ZLIB)

set_target_properties(
        FBXImport PROPERTIES
        PREFIX ""
        OUTPUT_NAME "FBXImport"
)

install(TARGETS FBXImport
        EXPORT FBXImportTarget
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(FILES ${fbx_HEADERS} DESTINATION include)