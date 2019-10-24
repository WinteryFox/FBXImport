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
        include/FBX/Scene.h
        include/FBX/Vector3.h
        include/FBX/Face.h
        include/FBX/Process.h)

add_library(FBXImport SHARED ${fbx_SOURCES} ${fbx_HEADERS})
target_include_directories(FBXImport PUBLIC ${ZLIB_INCLUDE_DIRS})

if (FBX_BUILD_ZLIB)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/contrib/zlib)
    target_link_libraries(FBXImport zlib)
else (FBX_BUILD_ZLIB)
    find_package(ZLIB REQUIRED)
    target_link_libraries(FBXImport ZLIB::ZLIB)
endif (FBX_BUILD_ZLIB)

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