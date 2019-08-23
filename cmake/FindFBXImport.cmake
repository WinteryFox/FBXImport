include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path(
            FBX_IMPORT_INCLUDE_DIR
            NAMES FBXImport.h
            PATHS
            $ENV{PROGRAMFILES}/include
            DOC "FBXImport include dir")

    find_library(
            FBX_IMPORT_LIBRARY
            NAMES FBXImport.dll
            PATHS
            $ENV{PROGRAMFILES}/lib)
else ()
    find_path(
            FBX_IMPORT_INCLUDE_DIR
            NAMES assimp/scene.h
            PATHS
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            DOC "FBXImport include dir")

    find_library(
            FBX_IMPORT_LIBRARY
            NAMES assimp
            PATHS
            /usr/lib64
            /usr/lib
            /usr/local/lib64
            /usr/local/lib
            /sw/lib
            /opt/local/lib
            ${ASSIMP_ROOT_DIR}/lib
            DOC "FBXImport library")
endif ()

find_package_handle_standard_args(FBXImport DEFAULT_MSG FBX_IMPORT_INCLUDE_DIR FBX_IMPORT_LIBRARY)

if (FBX_IMPORT_FOUND)
    set(FBX_IMPORT_LIBRARIES ${FBX_IMPORT_LIBRARY})
    set(FBX_IMPORT_INCLUDE_DIRS ${FBX_IMPORT_INCLUDE_DIR})
endif ()

mark_as_advanced(FBX_IMPORT_INCLUDE_DIR FBX_IMPORT_LIBRARY)