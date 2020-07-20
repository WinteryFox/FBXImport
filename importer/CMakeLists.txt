set(fbx_SOURCES
        src/Decoder.cpp
        src/Parser.cpp
        src/Stream.cpp
        src/TriangulateProcess.cpp)
set(fbx_HEADERS
        include/FBX/Decoder.h
        include/FBX/Node.h
        include/FBX/Parser.h
        include/FBX/Mesh.h
        include/FBX/FBXImport.h
        include/FBX/Span.h
        include/FBX/Stream.h
        include/FBX/Scene.h
        include/FBX/Vector3.h
        include/FBX/Face.h
        include/FBX/Process.h
        include/FBX/Texture.h
        include/FBX/Material.h
        include/FBX/Util.h
        include/FBX/Object.h
        include/FBX/Model.h
        include/FBX/Vector2.h
        include/FBX/Settings.h)

add_library(FBXImport ${fbx_SOURCES} ${fbx_HEADERS})

find_package(ZLIB REQUIRED)
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