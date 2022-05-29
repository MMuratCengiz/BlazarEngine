ADD_DEFINITIONS(-DDEBUG)

IF (WIN32)
    ADD_DEFINITIONS(-D_WIN32=1)
ENDIF()

SET(CMAKE_CXX_STANDARD 17)

IF(MSVC)
    ADD_COMPILE_OPTIONS(
        $<$<CONFIG:>:/MT>
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    )

    SET(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )

    IF (CMAKE_BUILD_TYPE MATCHES "Debug")
        FOREACH(CompilerFlag ${CompilerFlags})
            STRING(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        ENDFOREACH()
    ENDIF()
ENDIF()
