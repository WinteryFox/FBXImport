add_executable(FBXImportTest main.cpp)
target_include_directories(FBXImportTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/importer/include)
target_link_libraries(FBXImportTest FBXImport)

install(TARGETS FBXImportTest
        DESTINATION bin)