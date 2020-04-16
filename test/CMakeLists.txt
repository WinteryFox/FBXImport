add_executable(FBXImportTest main.cpp)
target_include_directories(FBXImportTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../importer/include)
target_link_libraries(FBXImportTest FBXImport)

file(COPY models/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

install(TARGETS FBXImportTest
        DESTINATION bin)