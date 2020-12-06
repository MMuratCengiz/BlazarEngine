cmake_minimum_required(VERSION 3.16)

project(BlazarEngine)

set(SHADER_ROOT_DIR ${CMAKE_SOURCE_DIR}/Shaders/)

function(add_shader SHADER_TYPE SHADER_NAME)
    set(INPUT_FILE "${SHADER_ROOT_DIR}GLSL/${SHADER_TYPE}/${SHADER_NAME}.glsl")
    set(OUTPUT_FILE "${SHADER_ROOT_DIR}SPIRV/${SHADER_TYPE}/${SHADER_NAME}.spv")

    #remember to add glslc to environment path
    execute_process(COMMAND glslc -fshader-stage=${SHADER_TYPE} ${INPUT_FILE} -o ${OUTPUT_FILE})
endfunction()

add_shader(vertex default)
add_shader(fragment default)
add_shader(vertex skybox_default)
add_shader(fragment skybox_default)

set(CMAKE_CXX_STANDARD 17)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("${CMAKE_SOURCE_DIR}/external/glm")
include_directories("${CMAKE_SOURCE_DIR}/external/stb")
include_directories("${CMAKE_SOURCE_DIR}/external/vma")

add_subdirectory("${CMAKE_SOURCE_DIR}/external/glfw")

include(${CMAKE_SOURCE_DIR}/cmake/include_assimp.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/include_blazar.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/include_bullet.cmake)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DENGINE_NAMESPACE=BlazarEngine)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DDEBUG)
add_definitions(-DG_VERBOSITY=3)
add_definitions(-DROOT_DIR="${CMAKE_SOURCE_DIR}")

find_package(Vulkan REQUIRED)

add_executable(BlazarEngine main.cpp ${Core} ${Graphics} ${Shaders} ${Input} ${ECS} ${RenderObjects} ${Scene} ${Samples} ${SPV})

target_compile_definitions(BlazarEngine PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

target_link_libraries(BlazarEngine
        glfw
        assimp
        Vulkan::Vulkan
        Bullet3Common
        Bullet3Geometry
        BulletSoftBody
        BulletDynamics
        Bullet3Dynamics
        Bullet3Collision
        BulletCollision
        LinearMath
        BulletInverseDynamics
        )
target_include_directories(BlazarEngine PUBLIC ${SPVDir})
