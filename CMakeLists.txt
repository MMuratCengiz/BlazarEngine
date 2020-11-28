cmake_minimum_required(VERSION 3.16)

project(SomeVulkan)

set(SHADER_ROOT_DIR ${CMAKE_SOURCE_DIR}/Shaders/)

function(add_shader SHADER_TYPE SHADER_NAME)
    set(INPUT_FILE "${SHADER_ROOT_DIR}GLSL/${SHADER_TYPE}/${SHADER_NAME}.glsl")
    set(OUTPUT_FILE "${SHADER_ROOT_DIR}SPIRV/${SHADER_TYPE}/${SHADER_NAME}.spv")

    #remember to add glslc to environment path
    execute_process(COMMAND glslc -fshader-stage=${SHADER_TYPE} ${INPUT_FILE} -o ${OUTPUT_FILE} )
endfunction()

add_shader(vertex default)
add_shader(fragment default)
add_shader(vertex   skybox_default)
add_shader(fragment skybox_default)

set(CMAKE_CXX_STANDARD 17)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set(Libs external/loaders/stb_image.h external/vma/vk_mem_alloc.h)

set(Core Core/Common.h Core/Time.h Scene/World.h)
set(Graphics Graphics/RenderDevice.cpp Graphics/RenderDevice.h Graphics/RenderSurface.cpp Graphics/RenderSurface.h Graphics/Renderer.cpp Graphics/Renderer.h Graphics/RenderDeviceBuilder.h Graphics/RenderDeviceBuilder.cpp Graphics/DrawDescription.h Graphics/RenderUtilities.h Graphics/InstanceContext.h Graphics/GraphicsException.h Core/Time.cpp Graphics/CommonTextures.h Graphics/DescriptorManager.cpp Graphics/DescriptorManager.h Graphics/CommanExecutorArguments.h Graphics/ClientSideLock.h Graphics/VmaImplementation.cpp Graphics/GLSLShaderSet.cpp Graphics/GLSLShaderSet.h "Graphics/MeshLoader.h" "Graphics/MeshLoader.cpp" Graphics/TextureLoader.cpp Graphics/TextureLoader.h Graphics/RenderUtilities.cpp Graphics/TransformLoader.cpp Graphics/TransformLoader.h Graphics/CameraLoader.cpp Graphics/CameraLoader.h Graphics/BuiltinPrimitives.h Graphics/PipelineSelector.h Graphics/CubeMapLoader.cpp Graphics/CubeMapLoader.h)
set(Input Input/GlobalEventHandler.cpp Input/GlobalEventHandler.h Core/DynamicMemory.h Input/ActionMap.cpp Input/ActionMap.h Input/EventHandler.cpp Input/EventHandler.h)
set(ECS ECS/IGameEntity.h ECS/IGameEntity.h ECS/Renderable.h ECS/IComponent.h ECS.h ECS/CMesh "ECS/ECSUtilities.h" "ECS/CMaterial.h" "ECS/ISystem.h" ECS/CTransform.h ECS/CCubeMap.h)
set(Scene Scene/Camera.h Scene/FpsCamera.h Scene/FpsCamera.cpp Scene/Scene.h Scene/FPSCounter.h Scene/IPlayable.h Scene/Window.h)
set(Samples Samples/SampleGame.h Samples/SampleGame.cpp Samples/SampleHouse.h Samples/SampleFloor.h Samples/SampleTrafficCone.h Samples/SampleCar1.h Samples/SampleCar2.h Samples/SampleOldHouse.h Samples/SampleCubeMap.h)

set(SampleGame Samples/SampleHouse.h)

set(Shaders Shaders/GLSL/Fragment/default.glsl Shaders/GLSL/Vertex/default.glsl Graphics/CommandExecutor.cpp Graphics/CommandExecutor.h)
set(SPVDir "${CMAKE_SOURCE_DIR}/external/SPIRV-Cross")
set(SPV ${SPVDir}/spirv.hpp ${SPVDir}/spirv_cfg.hpp ${SPVDir}/spirv_cfg.cpp ${SPVDir}/spirv_cpp.hpp ${SPVDir}/spirv_cpp.cpp ${SPVDir}/spirv_parser.hpp ${SPVDir}/spirv_parser.cpp ${SPVDir}/spirv_reflect.hpp ${SPVDir}/spirv_reflect.cpp ${SPVDir}/spirv_cross.hpp ${SPVDir}/spirv_cross.cpp ${SPVDir}/spirv_cross_util.hpp ${SPVDir}/spirv_cross_util.cpp ${SPVDir}/spirv_glsl.hpp ${SPVDir}/spirv_glsl.cpp ${SPVDir}/spirv_cross_parsed_ir.hpp ${SPVDir}/spirv_cross_parsed_ir.cpp)

include_directories("${CMAKE_SOURCE_DIR}/external/glm")
include_directories("${CMAKE_SOURCE_DIR}/external/stb") 
include_directories("${CMAKE_SOURCE_DIR}/external/vma")

add_subdirectory("${CMAKE_SOURCE_DIR}/external/glfw")
add_subdirectory("${CMAKE_SOURCE_DIR}/external/assimp")

include_directories(
        "${CMAKE_SOURCE_DIR}/external/assimp/include"
        "${CMAKE_SOURCE_DIR}/external/assimp/code"
)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DDEBUG)
add_definitions(-DG_VERBOSITY=3)
add_definitions(-DROOT_DIR="${CMAKE_SOURCE_DIR}")
            
find_package(Vulkan REQUIRED)

add_executable(SomeVulkan main.cpp ${Core} ${Graphics} ${Shaders} ${Input} ${ECS} ${RenderObjects} ${Scene} ${Samples} ${SPV} )

target_compile_definitions( SomeVulkan PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING  )

            
link_directories(SomeVulkan
        ${ASSIMP_BINARY_DIR}
        ${ASSIMP_BINARY_DIR}/lib)

target_link_libraries( SomeVulkan glfw assimp Vulkan::Vulkan )
target_include_directories( SomeVulkan PUBLIC ${SPVDir} )
