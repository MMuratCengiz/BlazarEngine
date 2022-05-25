INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/stb")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/miniz")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/vma/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/tinygltf")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/glm")

SET(GLFW_OPTIONS
        -DGLFW_BUILD_EXAMPLES:BOOL=OFF
        -DGLFW_BUILD_TESTS:BOOL=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGLFW_BUILD_DOCS:BOOL=OFF
        -DGLFW_INSTALL:BOOL=ON
        -DGLFW_VULKAN_STATIC:BOOL=ON
        ${CMAKE_ARGS_FOR_EXTERNALS}
        )

IF (WIN32)
    SET(GLFW_OPTIONS ${GLFW_OPTIONS}
        -DGLFW_EXPOSE_NATIVE_WIN32=1
        -DGLFW_EXPOSE_NATIVE_WGL=1)
ENDIF()

ExternalProject_Add(glfw
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/glfw"
        CMAKE_ARGS ${GLFW_OPTIONS}
        BUILD_ALWAYS 1
        BUILD_IN_SOURCE 1
        PREFIX "glfw"
        CMAKE_CACHE_ARGS "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}" "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
        CMAKE_GENERATOR "${CMAKE_GENERATOR}"
        )

IF (${BLAZAR_INSTALL_LIBS})
    INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/external/stb/" DESTINATION include
            FILES_MATCHING
            PATTERN "*.h")

    INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/external/miniz/" DESTINATION include
            FILES_MATCHING
            PATTERN "*.h")

    SET(VMA_FILES
            "${PROJECT_SOURCE_DIR}/external/vma/include/vk_mem_alloc.h"
            "${PROJECT_SOURCE_DIR}/external/vma/include/vk_mem_alloc.hpp"
            "${PROJECT_SOURCE_DIR}/external/vma/include/vk_mem_alloc_enums.hpp"
            "${PROJECT_SOURCE_DIR}/external/vma/include/vk_mem_alloc_funcs.hpp"
            "${PROJECT_SOURCE_DIR}/external/vma/include/vk_mem_alloc_handles.hpp"
            "${PROJECT_SOURCE_DIR}/external/vma/include/vk_mem_alloc_structs.hpp"
            )
    INSTALL(FILES ${VMA_FILES} DESTINATION include)

    SET(TINYGLTF_FILES
            "${PROJECT_SOURCE_DIR}/external/tinygltf/tiny_gltf.h"
            "${PROJECT_SOURCE_DIR}/external/tinygltf/json.hpp"
            )
    INSTALL(FILES ${TINYGLTF_FILES} DESTINATION include)

    INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/external/glm/" DESTINATION include
            FILES_MATCHING
            PATTERN "*.inl"
            PATTERN "*.h"
            PATTERN "*.hpp")
ENDIF ()

SET(BULLET_OPTIONS
        -DINSTALL_LIBS=ON
        -DCMAKE_INSTALL_LIBS=ON
        ${CMAKE_ARGS_FOR_EXTERNALS}
        "-DPKGCONFIG_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/lib"
        -DBUILD_BULLET2_DEMOS=OFF
        -DBUILD_CPU_DEMOS=OFF
        -DBUILD_EXTRAS=OFF
        -DBUILD_OPENGL3_DEMOS=OFF
        -DBUILD_UNIT_TESTS=OFF
        -DUSE_DOUBLE_PRECISION:BOOL=OFF
        "-DINCLUDE_INSTALL_DIR=${CMAKE_INSTALL_PREFIX}/include/"
        )

ExternalProject_Add(bullet3
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/bullet3"
        CMAKE_ARGS ${BULLET_OPTIONS}
        BUILD_ALWAYS 1
        BUILD_IN_SOURCE 1
        CMAKE_CACHE_ARGS "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}" "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
        PREFIX "bullet3"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        )

INCLUDE_DIRECTORIES("${CMAKE_INSTALL_PREFIX}/include")
LINK_DIRECTORIES("${CMAKE_INSTALL_PREFIX}/lib")