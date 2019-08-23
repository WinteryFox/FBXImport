find_package(ZLIB REQUIRED)

set(fbx_SOURCES
        src/Decoder.cpp
        src/Parser.cpp
        src/Stream.cpp)
set(fbx_HEADERS
        include/FBX/Decoder.h
        include/FBX/Node.h
        include/FBX/Parser.h
        include/FBX/Mesh.h
        include/FBX/FBXImport.h
        include/FBX/Span.h
        include/FBX/Stream.h
        include/FBX/Scene.h)

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
install(FILES ${fbx_HEADERS} DESTINATION include/FBX)