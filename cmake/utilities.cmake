FUNCTION(copy_to_binary Dir FileSelect)
    FILE(GLOB_RECURSE FilesInDir "${PROJECT_SOURCE_DIR}/${Dir}/${FileSelect}")
    STRING(LENGTH "${PROJECT_SOURCE_DIR}/${Dir}/" PathLen)

    FOREACH(File IN LISTS FilesInDir)
        GET_FILENAME_COMPONENT(FileParentDir ${File} DIRECTORY)
        SET(FileParentDir ${FileParentDir}/)

        STRING(LENGTH "${File}" FilePathLen)
        STRING(LENGTH "${FileParentDir}" DirPathLen)

        MATH(EXPR FileTrimmedLen "${FilePathLen}-${PathLen}")
        MATH(EXPR DirTrimmedLen "${FilePathLen}-${DirPathLen}")

        STRING(SUBSTRING ${File} ${PathLen} ${FileTrimmedLen} FileStripped)
        STRING(SUBSTRING ${FileParentDir} ${PathLen} ${DirTrimmedLen} DirStripped)

        FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/${Dir}")
        FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/${Dir}/${DirStripped}")

        CONFIGURE_FILE(${File} ${PROJECT_BINARY_DIR}/${Dir}/${DirStripped} COPYONLY)
    ENDFOREACH()
ENDFUNCTION()