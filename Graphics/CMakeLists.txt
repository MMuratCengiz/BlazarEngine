INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_definitions.cmake)

SET(BlazarGraphicsSources
        BlazarGraphics/GraphicsException.h
        BlazarGraphics/BuiltinPrimitives.h
        BlazarGraphics/GraphicsCommonIncludes.h
        BlazarGraphics/IRenderDevice.h
        BlazarGraphics/IPipelineProvider.h
        BlazarGraphics/IRenderPassProvider.h
        BlazarGraphics/IResourceProvider.h
        BlazarGraphics/RenderWindow.h
        BlazarGraphics/AssetManager.cpp
        BlazarGraphics/AssetManager.h
        BlazarGraphics/DataAttachmentFormatter.cpp
        BlazarGraphics/DataAttachmentFormatter.h
        # RenderGraphSources:
        BlazarGraphics/RenderGraph/StaticVars.h
        BlazarGraphics/RenderGraph/GlobalResourceTable.cpp
        BlazarGraphics/RenderGraph/GlobalResourceTable.h
        BlazarGraphics/RenderGraph/Pass.h
        BlazarGraphics/RenderGraph/RenderGraph.cpp
        BlazarGraphics/RenderGraph/RenderGraph.h
        BlazarGraphics/RenderGraph/CommonPasses.cpp
        BlazarGraphics/RenderGraph/CommonPasses.h
        BlazarGraphics/RenderGraph/GraphSystem.cpp
        BlazarGraphics/RenderGraph/GraphSystem.h
        # VulkanBackendSources:
        BlazarGraphics/VulkanBackend/VulkanDevice.h
        BlazarGraphics/VulkanBackend/VulkanDevice.cpp
        BlazarGraphics/VulkanBackend/VulkanSurface.h
        BlazarGraphics/VulkanBackend/VulkanSurface.cpp
        BlazarGraphics/VulkanBackend/VulkanUtilities.h
        BlazarGraphics/VulkanBackend/VulkanUtilities.cpp
        BlazarGraphics/VulkanBackend/VulkanContext.h
        BlazarGraphics/VulkanBackend/VulkanPipelineProvider.cpp
        BlazarGraphics/VulkanBackend/VulkanPipelineProvider.h
        BlazarGraphics/VulkanBackend/VulkanRenderPassProvider.cpp
        BlazarGraphics/VulkanBackend/VulkanRenderPassProvider.h
        BlazarGraphics/VulkanBackend/VulkanResourceProvider.cpp
        BlazarGraphics/VulkanBackend/VulkanResourceProvider.h
        BlazarGraphics/VulkanBackend/DescriptorManager.cpp
        BlazarGraphics/VulkanBackend/DescriptorManager.h
        BlazarGraphics/VulkanBackend/VulkanCommandExecutor.h
        BlazarGraphics/VulkanBackend/VulkanCommandExecutor.cpp
        BlazarGraphics/VulkanBackend/VulkanSamplerAllocator.h
        BlazarGraphics/VulkanBackend/VulkanSamplerAllocator.cpp
        BlazarGraphics/VulkanBackend/CommandExecutorArguments.h
        BlazarGraphics/VulkanBackend/VmaImplementation.cpp
        BlazarGraphics/VulkanBackend/GLSLShaderSet.h
        BlazarGraphics/VulkanBackend/GLSLShaderSet.cpp
        BlazarGraphics/VulkanBackend/VulkanCubeMapAllocator.cpp
        BlazarGraphics/VulkanBackend/VulkanCubeMapAllocator.h )

ADD_LIBRARY(BlazarGraphics ${BlazarGraphicsSources})

SET_TARGET_PROPERTIES(BlazarGraphics PROPERTIES LINKER_LANGUAGE CXX)

SET(INCLUDE_BOOST_TARGET BlazarGraphics)
SET(REQUESTED_BOOST_LIBS thread)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_boost.cmake)

TARGET_LINK_LIBRARIES(BlazarGraphics
        assimp
        glfw
        Vulkan::Vulkan
        BlazarCore
        BlazarECS
        BlazarInput
        )