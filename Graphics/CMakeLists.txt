SET(BlazarGraphicsSources
        BlazarGraphics/RenderDevice.h
        BlazarGraphics/RenderDevice.cpp
        BlazarGraphics/RenderSurface.h
        BlazarGraphics/RenderSurface.cpp
        BlazarGraphics/Renderer.h
        BlazarGraphics/Renderer.cpp
        BlazarGraphics/RenderDeviceBuilder.h
        BlazarGraphics/RenderDeviceBuilder.cpp
        BlazarGraphics/DrawDescription.h
        BlazarGraphics/RenderUtilities.h
        BlazarGraphics/RenderUtilities.cpp
        BlazarGraphics/InstanceContext.h
        BlazarGraphics/GraphicsException.h
        BlazarGraphics/CommonTextures.h
        BlazarGraphics/DescriptorManager.cpp
        BlazarGraphics/DescriptorManager.h
        BlazarGraphics/CommandExecutor.h
        BlazarGraphics/CommandExecutor.cpp
        BlazarGraphics/CommandExecutorArguments.h
        BlazarGraphics/ClientSideLock.h
        BlazarGraphics/VmaImplementation.cpp
        BlazarGraphics/GLSLShaderSet.h
        BlazarGraphics/GLSLShaderSet.cpp
        BlazarGraphics/MeshLoader.h
        BlazarGraphics/MeshLoader.cpp
        BlazarGraphics/MaterialLoader.h
        BlazarGraphics/MaterialLoader.cpp
        BlazarGraphics/TransformLoader.h
        BlazarGraphics/TransformLoader.cpp
        BlazarGraphics/CameraLoader.h
        BlazarGraphics/CameraLoader.cpp
        BlazarGraphics/BuiltinPrimitives.h
        BlazarGraphics/PipelineSelector.h
        BlazarGraphics/CubeMapLoader.h
        BlazarGraphics/CubeMapLoader.cpp
        BlazarGraphics/LightLoader.h
        BlazarGraphics/LightLoader.cpp
        BlazarGraphics/WorldContextLoader.h
        BlazarGraphics/WorldContextLoader.cpp
        BlazarGraphics/GraphicsCommonIncludes.h)

ADD_LIBRARY(BlazarGraphics ${BlazarGraphicsSources})

SET_TARGET_PROPERTIES(BlazarGraphics PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(BlazarGraphics
        assimp
        glfw
        Vulkan::Vulkan
        BlazarCore
        BlazarECS
        BlazarInput
        )