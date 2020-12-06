set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_SOURCE_DIR}/external/assimp")

include_directories(
        "${CMAKE_SOURCE_DIR}/external/assimp/include"
        "${CMAKE_SOURCE_DIR}/external/assimp/code"
        "${ASSIMP_BINARY_DIR}/include"
)

link_directories(BlazarEngine
        ${ASSIMP_BINARY_DIR}
        ${ASSIMP_BINARY_DIR}/lib)