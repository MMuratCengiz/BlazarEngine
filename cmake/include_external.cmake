INCLUDE(${PROJECT_SOURCE_DIR}/cmake/utilities.cmake)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/stb")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/miniz")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/vma/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/tinygltf")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/glm")

LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/stb")
LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/miniz")
LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/vma/include")
LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/tinygltf")

SET(GLFW_OPTIONS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DGLFW_BUILD_EXAMPLES:BOOL=OFF
        -DGLFW_BUILD_TESTS:BOOL=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGLFW_BUILD_DOCS:BOOL=OFF
        -DGLFW_INSTALL:BOOL=ON
        -DGLFW_VULKAN_STATIC:BOOL=ON
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
        -DCMAKE_C_FLAGS_RELWITHDEBINFO=${CMAKE_C_FLAGS_RELWITHDEBINFO}
        -DCMAKE_C_FLAGS_MINSIZEREL=${CMAKE_C_FLAGS_MINSIZEREL}
        -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
        -DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}
        -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>
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

LINK_DIRECTORIES(${BLAZAR_INSTALL_LOCATION}lib)
INCLUDE_DIRECTORIES(${BLAZAR_INSTALL_LOCATION}include)

IF (${BLAZAR_INSTALL_LIBS})
    INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/external/stb/" DESTINATION ${BLAZAR_INSTALL_LOCATION}include
            FILES_MATCHING
            PATTERN "*.h")

    INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/external/miniz/" DESTINATION ${BLAZAR_INSTALL_LOCATION}include
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
    INSTALL(FILES ${VMA_FILES} DESTINATION ${BLAZAR_INSTALL_LOCATION}include)

    SET(TINYGLTF_FILES
            "${PROJECT_SOURCE_DIR}/external/tinygltf/tiny_gltf.h"
            "${PROJECT_SOURCE_DIR}/external/tinygltf/json.hpp"
            )
    INSTALL(FILES ${TINYGLTF_FILES} DESTINATION ${BLAZAR_INSTALL_LOCATION}include)

    INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/external/glm/" DESTINATION ${BLAZAR_INSTALL_LOCATION}include
            FILES_MATCHING
            PATTERN "*.inl"
            PATTERN "*.h"
            PATTERN "*.hpp")
ENDIF ()

# IMPORT LUA #################################################
ExternalProject_Add(lua
        URL "${PROJECT_SOURCE_DIR}/external/lua-5.4.4.tar.gz"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make generic
        BUILD_ALWAYS true
        BUILD_IN_SOURCE true
        INSTALL_COMMAND ""
        PREFIX "lua"
        )

ExternalProject_Get_property(lua SOURCE_DIR)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/src)
SET(LUA_LIB ${SOURCE_DIR}/src/liblua.a)
###############################################################

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/external/sol3)

COPY_TO_BINARY("LuaSample" "*.lua")