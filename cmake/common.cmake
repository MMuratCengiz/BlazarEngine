INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_definitions.cmake)

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

FUNCTION(INSTALL_TARGET target)
    IF (BLAZAR_INSTALL_LIBS)
        INSTALL(TARGETS ${target}
                EXPORT ${target}-export
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib
                )

        INSTALL(EXPORT ${target}-export
                FILE ${target}Targets.cmake
                NAMESPACE ${target}::
                DESTINATION cmake/${target}
                )

        INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
    ENDIF()
ENDFUNCTION()

FUNCTION(TARGET_INCLUDE_DEFAULT_DIRECTORIES target)
    TARGET_INCLUDE_DIRECTORIES(${target}
            PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
            )
ENDFUNCTION()

SET(CMAKE_ARGS_FOR_EXTERNALS
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_C_COMPILER_LAUNCHER:FILEPATH=${CMAKE_C_COMPILER_LAUNCHER}"
        "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
        "-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}"
        "-DCMAKE_CXX_COMPILER_LAUNCHER:FILEPATH=${CMAKE_CXX_COMPILER_LAUNCHER}"
        "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
        "-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}"
        "-DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}"
        "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
        "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_INSTALL_PREFIX}"
        "-DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}"
        "-DCMAKE_VERBOSE_MAKEFILE:BOOL=${CMAKE_VERBOSE_MAKEFILE}"
        "-DCMAKE_MODULE_PATH:STRING=${CMAKE_MODULE_PATH}"
        )