add_executable(FBXImportTest main.cpp)
target_include_directories(FBXImportTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../importer/include)
target_link_libraries(FBXImportTest FBXImport)

configure_file(models/fox.fbx ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Fox.FBX COPYONLY)

install(TARGETS FBXImportTest
        DESTINATION bin)