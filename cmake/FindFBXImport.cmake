include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path(
            FBX_IMPORT_INCLUDE_DIR
            NAMES FBX/FBXImport.h
            PATHS
            "$ENV{ProgramFiles}/FBXImport/include"
            "$ENV{ProgramFiles\(x86\)}/FBXImport/include"
            DOC "FBXImport include dir")
    find_library(
            FBX_IMPORT_LIBRARY
            NAMES FBXImport
            PATHS
            "$ENV{PROGRAMFILES}/FBXImport/lib"
            "$ENV{ProgramFiles\(x86\)}/FBXImport/lib")
else ()
    find_path(
            FBX_IMPORT_INCLUDE_DIR
            NAMES FBX/FBXImport.h
            PATHS
            "/usr/include"
            "/usr/local/include"
            "/sw/include"
            "/opt/local/include"
            DOC "FBXImport include dir")
    find_library(
            FBX_IMPORT_LIBRARY
            NAMES FBXImport
            PATHS
            "/usr/lib64"
            "/usr/lib"
            "/usr/local/lib64"
            "/usr/local/lib"
            "/sw/lib"
            "/opt/local/lib"
            DOC "FBXImport library")
endif ()

find_package_handle_standard_args(FBXImport DEFAULT_MSG FBX_IMPORT_INCLUDE_DIR FBX_IMPORT_LIBRARY)

if (FBX_IMPORT_FOUND)
    set(FBX_IMPORT_LIBRARIES ${FBX_IMPORT_LIBRARY})
    set(FBX_IMPORT_INCLUDE_DIRS ${FBX_IMPORT_INCLUDE_DIR})
endif ()

mark_as_advanced(FBX_IMPORT_INCLUDE_DIR FBX_IMPORT_LIBRARY)