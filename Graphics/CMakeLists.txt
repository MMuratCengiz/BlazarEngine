INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glfw_libs.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(glfw3 CONFIG REQUIRED)
FIND_PACKAGE(glslang CONFIG REQUIRED)

FIND_PACKAGE(spirv_cross_core CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_glsl CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_reflect CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_util CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_cpp CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_hlsl CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_msl CONFIG REQUIRED)
FIND_PACKAGE(spirv_cross_c CONFIG REQUIRED)

FIND_PATH(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

SET(BlazarGraphicsSources
        src/BlazarGraphics/AssetManager.cpp
        src/BlazarGraphics/DataAttachmentFormatter.cpp
        src/BlazarGraphics/RenderGraph/GlobalResourceTable.cpp
        src/BlazarGraphics/RenderGraph/RenderGraph.cpp
        src/BlazarGraphics/RenderGraph/CommonPasses.cpp
        src/BlazarGraphics/RenderGraph/GraphSystem.cpp
        src/BlazarGraphics/VulkanBackend/VulkanDevice.cpp
        src/BlazarGraphics/VulkanBackend/VulkanSurface.cpp
        src/BlazarGraphics/VulkanBackend/VulkanUtilities.cpp
        src/BlazarGraphics/VulkanBackend/VulkanPipelineProvider.cpp
        src/BlazarGraphics/VulkanBackend/VulkanRenderPassProvider.cpp
        src/BlazarGraphics/VulkanBackend/VulkanResourceProvider.cpp
        src/BlazarGraphics/VulkanBackend/DescriptorManager.cpp
        src/BlazarGraphics/VulkanBackend/VulkanCommandExecutor.cpp
        src/BlazarGraphics/VulkanBackend/VulkanSamplerAllocator.cpp
        src/BlazarGraphics/VulkanBackend/VmaImplementation.cpp
        src/BlazarGraphics/VulkanBackend/GLSLShaderSet.cpp
        src/BlazarGraphics/VulkanBackend/VulkanCubeMapAllocator.cpp
        src/BlazarGraphics/VulkanBackend/SpirvHelper.cpp
        src/BlazarGraphics/AnimationStateSystem.cpp
        src/BlazarGraphics/RenderGraph/ShaderUniformBinder.cpp)

ADD_LIBRARY(BlazarGraphics ${BLAZAR_LIB_TYPE} ${BlazarGraphicsHeaders} ${BlazarGraphicsSources})

INSTALL_TARGET(BlazarGraphics)
INSTALL(FILES ${SPVHeaders} DESTINATION include/BlazarGraphics)

TARGET_INCLUDE_DIRECTORIES(BlazarGraphics PRIVATE ${TINYGLTF_INCLUDE_DIRS})
TARGET_INCLUDE_DEFAULT_DIRECTORIES(BlazarGraphics)
TARGET_LINK_LIBRARIES(BlazarGraphics
        PUBLIC
            glfw
            ${Vulkan_LIBRARY}
            HLSL
            SPIRV
            glslang
            OGLCompiler
            BlazarCore
            BlazarECS
            BlazarInput
            spirv-cross-glsl
            spirv-cross-hlsl
            spirv-cross-cpp
            spirv-cross-reflect
            spirv-cross-msl
            spirv-cross-util
            spirv-cross-core
        )

SET_TARGET_PROPERTIES(BlazarGraphics PROPERTIES LINKER_LANGUAGE CXX)
