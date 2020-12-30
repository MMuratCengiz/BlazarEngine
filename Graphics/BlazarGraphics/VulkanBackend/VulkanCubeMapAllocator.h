#pragma once

#include <BlazarECS/ECS.h>
#include "VulkanContext.h"
#include "VulkanCommandExecutor.h"
#include "VulkanSamplerAllocator.h"
#include "VulkanUtilities.h"
#include "../AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct CubeMapLoadArguments
{
    const VulkanContext* vulkanContext;
    std::shared_ptr< VulkanCommandExecutor > commandExecutor;

    std::shared_ptr< CubeMapDataAttachment > image;

    explicit CubeMapLoadArguments( const VulkanContext* context ) : vulkanContext( context ) { }
};

class VulkanCubeMapAllocator
{
public:
    static void load( const CubeMapLoadArguments &arguments, VulkanTextureWrapper * target );
};

END_NAMESPACES