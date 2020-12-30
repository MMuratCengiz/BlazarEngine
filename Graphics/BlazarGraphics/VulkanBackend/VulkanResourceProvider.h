#pragma once

#include <BlazarCore/Common.h>
#include "VulkanCommandExecutor.h"
#include "VulkanUtilities.h"
#include "VulkanContext.h"
#include "VulkanSamplerAllocator.h"
#include "VulkanCubeMapAllocator.h"
#include "../IResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct VulkanBufferWrapper
{
    std::pair< vk::Buffer, vma::Allocation > buffer;
};

class VulkanResourceLock : public IResourceLock
{
private:
    vk::Fence fence { };
    vk::Semaphore semaphore { };

    VulkanContext* context;
public:
    VulkanResourceLock( VulkanContext* context, const ResourceLockType &lockType );
    void wait( ) override;
    void reset( ) override;
    void notify( ) override;

    const vk::Fence &getVkFence( );
    const vk::Semaphore &getVkSemaphore( );
    ~VulkanResourceLock( ) override;
};

class VulkanResourceProvider : public IResourceProvider
{
    VulkanContext * context;
    std::shared_ptr< VulkanCommandExecutor > commandExecutor;
public:
    inline explicit VulkanResourceProvider( VulkanContext * context ) : context( context )
    {
        commandExecutor = std::make_shared< VulkanCommandExecutor >( this->context );
    }

    std::shared_ptr< ShaderResource > createResource( const ShaderResourceRequest &request ) override;
    std::shared_ptr< IResourceLock > createLock( const ResourceLockType &lockType ) override;

    void createBufferAllocator( const std::shared_ptr< ShaderResource > &resource );
    void createSampler2DAllocator( const std::shared_ptr< ShaderResource > &resource );
    void createCubeMapAllocator( const std::shared_ptr< ShaderResource > &resource );

    ~VulkanResourceProvider( ) override = default;
};

END_NAMESPACES