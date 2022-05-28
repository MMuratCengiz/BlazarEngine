INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

LIST(APPEND ExternalDependencies_Includes external/stb)
LIST(APPEND ExternalDependencies_Includes external/miniz)
LIST(APPEND ExternalDependencies_Includes external/vma/include)
LIST(APPEND ExternalDependencies_Includes external/tinygltf)
LIST(APPEND ExternalDependencies_Includes external/glm)

IF (WIN32)
    SET(GLFW_OPTIONS ${GLFW_OPTIONS}
        -DGLFW_EXPOSE_NATIVE_WIN32=1
        -DGLFW_EXPOSE_NATIVE_WGL=1)
ENDIF()

IF (BLAZAR_INSTALL_LIBS)
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

FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(glslang CONFIG REQUIRED)

FIND_PACKAGE(Bullet CONFIG REQUIRED)
FIND_PACKAGE(glfw3 CONFIG REQUIRED)

STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" Bullet_INSTALL_INCLUDE_DIR ${BULLET_INCLUDE_DIR})

LIST(APPEND ExternalDependencies_Includes ${Bullet_INSTALL_INCLUDE_DIR})

LIST(APPEND ExternalDependencies_Libraries
        ${BULLET_LIBRARIES}
        glfw
        ${Vulkan_LIBRARY}
        )
