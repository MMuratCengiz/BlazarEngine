INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_definitions.cmake)

SET(BlazarGraphicsHeaders
        BlazarGraphics/GraphicsException.h
        BlazarGraphics/BuiltinPrimitives.h
        BlazarGraphics/GraphicsCommonIncludes.h
        BlazarGraphics/IRenderDevice.h
        BlazarGraphics/IPipelineProvider.h
        BlazarGraphics/IRenderPassProvider.h
        BlazarGraphics/IResourceProvider.h
        BlazarGraphics/RenderWindow.h
        BlazarGraphics/AssetManager.h
        BlazarGraphics/DataAttachmentFormatter.h
        # RenderGraphSources:
        BlazarGraphics/RenderGraph/StaticVars.h
        BlazarGraphics/RenderGraph/GlobalResourceTable.h
        BlazarGraphics/RenderGraph/Pass.h
        BlazarGraphics/RenderGraph/RenderGraph.h
        BlazarGraphics/RenderGraph/CommonPasses.h
        BlazarGraphics/RenderGraph/GraphSystem.h
        # VulkanBackendSources:
        BlazarGraphics/VulkanBackend/VulkanDevice.h
        BlazarGraphics/VulkanBackend/VulkanSurface.h
        BlazarGraphics/VulkanBackend/VulkanUtilities.h
        BlazarGraphics/VulkanBackend/VulkanContext.h
        BlazarGraphics/VulkanBackend/VulkanPipelineProvider.h
        BlazarGraphics/VulkanBackend/VulkanRenderPassProvider.h
        BlazarGraphics/VulkanBackend/VulkanResourceProvider.h
        BlazarGraphics/VulkanBackend/DescriptorManager.h
        BlazarGraphics/VulkanBackend/VulkanCommandExecutor.h
        BlazarGraphics/VulkanBackend/VulkanSamplerAllocator.h
        BlazarGraphics/VulkanBackend/CommandExecutorArguments.h
        BlazarGraphics/VulkanBackend/GLSLShaderSet.h
        BlazarGraphics/VulkanBackend/VulkanCubeMapAllocator.h
        BlazarGraphics/AnimationStateSystem.h
        BlazarGraphics/RenderGraph/ShaderUniformBinder.h)

SET(BlazarGraphicsSources
        BlazarGraphics/AssetManager.cpp
        BlazarGraphics/DataAttachmentFormatter.cpp
        # RenderGraphSources:
        BlazarGraphics/RenderGraph/GlobalResourceTable.cpp
        BlazarGraphics/RenderGraph/RenderGraph.cpp
        BlazarGraphics/RenderGraph/CommonPasses.cpp
        BlazarGraphics/RenderGraph/GraphSystem.cpp
        # VulkanBackendSources:
        BlazarGraphics/VulkanBackend/VulkanDevice.cpp
        BlazarGraphics/VulkanBackend/VulkanSurface.cpp
        BlazarGraphics/VulkanBackend/VulkanUtilities.cpp
        BlazarGraphics/VulkanBackend/VulkanPipelineProvider.cpp
        BlazarGraphics/VulkanBackend/VulkanRenderPassProvider.cpp
        BlazarGraphics/VulkanBackend/VulkanResourceProvider.cpp
        BlazarGraphics/VulkanBackend/DescriptorManager.cpp
        BlazarGraphics/VulkanBackend/VulkanCommandExecutor.cpp
        BlazarGraphics/VulkanBackend/VulkanSamplerAllocator.cpp
        BlazarGraphics/VulkanBackend/VmaImplementation.cpp
        BlazarGraphics/VulkanBackend/GLSLShaderSet.cpp
        BlazarGraphics/VulkanBackend/VulkanCubeMapAllocator.cpp
        BlazarGraphics/AnimationStateSystem.cpp
        BlazarGraphics/RenderGraph/ShaderUniformBinder.cpp)

ADD_LIBRARY(BlazarGraphics ${BLAZAR_LIB_TYPE} ${BlazarGraphicsHeaders} ${BlazarGraphicsSources})

IF (DEFINED BLAZAR_INSTALL_LIBS)
    INSTALL(TARGETS BlazarGraphics DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${BlazarGraphicsHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include)
ENDIF()

SET_TARGET_PROPERTIES(BlazarGraphics PROPERTIES LINKER_LANGUAGE CXX)

SET(INCLUDE_BOOST_TARGET BlazarGraphics)
SET(REQUESTED_BOOST_LIBS format atomic log)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

TARGET_LINK_LIBRARIES(BlazarGraphics
        glm
        glfw
        Vulkan::Vulkan
        BlazarCore
        BlazarECS
        BlazarInput
        )