cmake_minimum_required(VERSION 3.16)

project(SomeVulkan)

set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/CompilerOptions.cmake")
set(SHADER_ROOT_DIR ${CMAKE_SOURCE_DIR}/shaders/)

function(add_shader SHADER_TYPE SHADER_NAME)
    set(INPUT_FILE "${SHADER_ROOT_DIR}glsl/${SHADER_TYPE}/${SHADER_NAME}.glsl")
    set(OUTPUT_FILE "${SHADER_ROOT_DIR}spirv/${SHADER_TYPE}/${SHADER_NAME}.spv")

    #remember to add glslc to environment path
    execute_process(COMMAND glslc -fshader-stage=${SHADER_TYPE} ${INPUT_FILE} -o ${OUTPUT_FILE} )
endfunction()

add_shader(vertex default)
add_shader(fragment default)

set(CMAKE_CXX_STANDARD 20)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

set(Libs external/loaders/stb_image.h external/vma/vk_mem_alloc.h)

set(Core core/Game.h core/Common.h core/Time.h)
set(Graphics graphics/RenderDevice.cpp graphics/RenderDevice.h graphics/RenderSurface.cpp graphics/RenderSurface.h graphics/Renderer.cpp graphics/Renderer.h graphics/RenderDeviceBuilder.h graphics/RenderDeviceBuilder.cpp graphics/DefaultShaderLayout.h graphics/DrawDescription.h graphics/RenderUtilities.h graphics/InstanceContext.h graphics/GraphicsException.h core/Time.cpp graphics/RendererTypes.h graphics/ShaderLayout.h graphics/CommonTextures.h graphics/DescriptorManager.cpp graphics/DescriptorManager.h graphics/Texture.h graphics/Texture.cpp graphics/CommanExecutorArguments.h graphics/ClientSideLock.h graphics/VmaImplementation.cpp graphics/RenderPipeline.cpp graphics/RenderPipeline.h graphics/GLSLShader.cpp graphics/GLSLShader.h)
set(Input input/GlobalEventHandler.cpp input/GlobalEventHandler.h core/DynamicMemory.h)
set(ECS ecs/IGameEntity.h ecs/IGameEntity.h ecs/Renderable.h ecs/IComponent.h ECS.h ecs/Renderable.cpp)
set(RenderObjects renderobjects/Triangle2D.h renderobjects/Model.h)

set(Shaders shaders/glsl/fragment/default.glsl shaders/glsl/vertex/default.glsl renderobjects/Triangle2D.h graphics/CommandExecutor.cpp graphics/CommandExecutor.h)

add_subdirectory(./external/glfw)
add_subdirectory(./external/glm)
add_subdirectory(./external/assimp)

ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-DDEBUG)
ADD_DEFINITIONS(-DG_VERBOSITY=3)
ADD_DEFINITIONS(-DROOT_DIR="${CMAKE_SOURCE_DIR}")

include_directories(
        external
        external/assimp/include
        external/assimp/code
)

find_package(Vulkan REQUIRED)

add_executable(SomeVulkan main.cpp ${Core} ${Graphics} ${Shaders} ${Input} ${ECS} ${RenderObjects})

#target_compile_options( SomeVulkan PRIVATE /w )
target_compile_definitions( SomeVulkan PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING )

link_directories(SomeVulkan
        ${Assimp_BINARY_DIR}
        ${Assimp_BINARY_DIR}/lib)

target_link_libraries(SomeVulkan glfw assimp Vulkan::Vulkan)
