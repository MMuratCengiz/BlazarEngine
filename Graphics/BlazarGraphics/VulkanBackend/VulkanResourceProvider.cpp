#include "VulkanResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< ShaderResource > VulkanResourceProvider::createResource( const ShaderResourceRequest &request )
{
    auto resource = std::make_shared< ShaderResource >( );

    resource->type = request.type;
    resource->shaderStage = request.shaderStage;
    resource->loadStrategy = request.loadStrategy;
    resource->persistStrategy = request.persistStrategy;

    switch ( request.type )
    {
        case ResourceType::VertexData:
        case ResourceType::IndexData:
        case ResourceType::Uniform:
            createBufferAllocator( resource );
            break;
        case ResourceType::Sampler2D:
            createSampler2DAllocator( resource );
            break;
        case ResourceType::Sampler3D:
            break;
        case ResourceType::CubeMap:
            createCubeMapAllocator( resource );
            break;
    }

    return resource;
}

void VulkanResourceProvider::createBufferAllocator( const std::shared_ptr< ShaderResource > &resource )
{
    resource->allocate = [ = ]( )
    {
        std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

        uint64_t size = resource->dataAttachment->size;
        const void *content = resource->dataAttachment->content;

        if ( resource->loadStrategy == ResourceLoadStrategy::LoadOnce || resource->loadStrategy == ResourceLoadStrategy::LoadOnUpdate )
        {
            VulkanUtilities::initStagingBuffer( context, stagingBuffer, content, size );
        }

        vk::BufferCreateInfo bufferCreateInfo;
        if ( resource->type == ResourceType::Uniform )
        {
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
        }
        else if ( resource->type == ResourceType::VertexData )
        {
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
        }
        else if ( resource->type == ResourceType::IndexData )
        {
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
        }


        bufferCreateInfo.size = size;
        bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vma::AllocationCreateInfo allocationInfo;

        if ( resource->loadStrategy == ResourceLoadStrategy::LoadPerFrame )
        {
            allocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
            allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
            allocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal;
        }
        else
        {
            bufferCreateInfo.usage = bufferCreateInfo.usage | vk::BufferUsageFlagBits::eTransferDst;

            allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
            allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        }

        resource->apiSpecificBuffer = new VulkanBufferWrapper {
                this->context->vma.createBuffer( bufferCreateInfo, allocationInfo )
        };

        auto &buffer = reinterpret_cast< VulkanBufferWrapper * >( resource->apiSpecificBuffer )->buffer;

        if ( resource->loadStrategy == ResourceLoadStrategy::LoadOnce || resource->loadStrategy == ResourceLoadStrategy::LoadOnUpdate )
        {
            commandExecutor->startCommandExecution( )
                    ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
                    ->beginCommand( )
                    ->copyBuffer( size, stagingBuffer.first, buffer.first )
                    ->execute( );

            context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
        }
        else
        {
            auto deviceMemory = this->context->vma.mapMemory( buffer.second );

            memcpy( deviceMemory, content, size );

            this->context->vma.unmapMemory( buffer.second );
        }
    };

    resource->update = [ = ]( )
    {
        auto &buffer = reinterpret_cast< VulkanBufferWrapper * >( resource->apiSpecificBuffer )->buffer;

        auto deviceMemory = this->context->vma.mapMemory( buffer.second );

        memcpy( deviceMemory, resource->dataAttachment->content, resource->dataAttachment->size );

        this->context->vma.unmapMemory( buffer.second );
    };

    resource->deallocate = [ = ]( )
    {
        auto *pWrapper = reinterpret_cast< VulkanBufferWrapper * >( resource->apiSpecificBuffer );

        auto &buffer = pWrapper->buffer;

        context->vma.destroyBuffer( buffer.first, buffer.second );

        delete pWrapper;
    };
}

void VulkanResourceProvider::createSampler2DAllocator( const std::shared_ptr< ShaderResource > &resource )
{
    resource->allocate = [ = ]( )
    {
        resource->apiSpecificBuffer = new VulkanTextureWrapper { };

        TextureLoadArguments loadArguments { };
        loadArguments.context = context;
        loadArguments.commandExecutor = commandExecutor;

        const std::shared_ptr< SamplerDataAttachment > &samplerAttachment = std::dynamic_pointer_cast< SamplerDataAttachment >( resource->dataAttachment );

        if ( samplerAttachment->width == 0 )
        {
            samplerAttachment->width = context->surfaceExtent.width;
        }
        if ( samplerAttachment->height == 0 )
        {
            samplerAttachment->height = context->surfaceExtent.height;
        }

        loadArguments.image = samplerAttachment;
        VulkanSamplerAllocator::load( loadArguments, reinterpret_cast< VulkanTextureWrapper * >( resource->apiSpecificBuffer ) );
    };

    resource->update = [ = ]( )
    {
    };

    resource->prepareForUsage = [ = ]( const ResourceUsage &usage )
    {
        auto *pWrapper = reinterpret_cast< VulkanTextureWrapper * >( resource->apiSpecificBuffer );
        VulkanUtilities::prepareImageForUsage( commandExecutor.get( ), pWrapper, usage );
    };

    resource->deallocate = [ = ]( )
    {
        auto *pWrapper = reinterpret_cast< VulkanTextureWrapper * >( resource->apiSpecificBuffer );

        auto &buffer = pWrapper->image;

        context->vma.destroyImage( pWrapper->image, pWrapper->allocation );
        context->logicalDevice.destroyImageView( pWrapper->imageView );
        context->logicalDevice.destroySampler( pWrapper->sampler );

        delete pWrapper;
    };
}

void VulkanResourceProvider::createCubeMapAllocator( const std::shared_ptr< ShaderResource > &resource )
{
    resource->allocate = [ = ]( )
    {
        resource->apiSpecificBuffer = new VulkanTextureWrapper { };

        CubeMapLoadArguments loadArguments { context };
        loadArguments.commandExecutor = commandExecutor;

        const std::shared_ptr< CubeMapDataAttachment > &cubeMapDataAttachment = std::dynamic_pointer_cast< CubeMapDataAttachment >( resource->dataAttachment );
        loadArguments.image = cubeMapDataAttachment;

        VulkanCubeMapAllocator::load( loadArguments, reinterpret_cast< VulkanTextureWrapper * >( resource->apiSpecificBuffer ) );
    };

    resource->update = [ = ]( )
    { };

    resource->prepareForUsage = [ = ]( const ResourceUsage &usage )
    { };

    resource->deallocate = [ = ]( )
    {
        auto *pWrapper = reinterpret_cast< VulkanTextureWrapper * >( resource->apiSpecificBuffer );

        auto &buffer = pWrapper->image;

        context->vma.destroyImage( pWrapper->image, pWrapper->allocation );
        context->logicalDevice.destroyImageView( pWrapper->imageView );
        context->logicalDevice.destroySampler( pWrapper->sampler );

        delete pWrapper;
    };
}

std::shared_ptr< IResourceLock > VulkanResourceProvider::createLock( const ResourceLockType &lockType )
{
    return std::make_shared< VulkanResourceLock >( context, lockType );
}

VulkanResourceLock::VulkanResourceLock( VulkanContext *context, const ResourceLockType &lockType ) : IResourceLock( lockType ), context( context )
{
    if ( lockType == ResourceLockType::Fence )
    {
        vk::FenceCreateInfo fenceCreateInfo { };
        fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        fence = this->context->logicalDevice.createFence( fenceCreateInfo );
    }
    else
    {
        vk::SemaphoreCreateInfo semaphoreCreateInfo { };
        semaphore = this->context->logicalDevice.createSemaphore( semaphoreCreateInfo );
    }
}

void BlazarEngine::Graphics::VulkanResourceLock::wait( )
{
    if ( lockType == ResourceLockType::Fence )
    {
        auto result = context->logicalDevice.waitForFences( 1, &fence, true, UINT64_MAX );
        VkCheckResult( result );
    }
    else
    {
        vk::SemaphoreWaitInfo waitInfo { };
        waitInfo.semaphoreCount = 1;
        waitInfo.pSemaphores = &semaphore;
        waitInfo.flags = vk::SemaphoreWaitFlagBits::eAny;

        auto result = context->logicalDevice.waitSemaphores( waitInfo, UINT64_MAX );
        VkCheckResult( result );
    }
}

void VulkanResourceLock::reset( )
{
    if ( lockType == ResourceLockType::Fence )
    {
        vk::Result result = context->logicalDevice.resetFences( 1, &fence );

        if ( result != vk::Result::eSuccess )
        {

        }
    }
    else
    {
        // No matching functionality
    }
}

void BlazarEngine::Graphics::VulkanResourceLock::notify( )
{
    if ( lockType == ResourceLockType::Fence )
    {
        // No notify on client
    }
    else
    {
        context->logicalDevice.signalSemaphore( semaphore );
    }
}

VulkanResourceLock::~VulkanResourceLock( )
{
    VulkanResourceLock::cleanup( );
}

const vk::Fence &VulkanResourceLock::getVkFence( )
{
    return fence;
}

const vk::Semaphore &VulkanResourceLock::getVkSemaphore( )
{
    return semaphore;
}

void VulkanResourceLock::cleanup( )
{
    FUNCTION_BREAK( resourceCleaned )
    resourceCleaned = true;

    if ( lockType == ResourceLockType::Fence )
    {
        context->logicalDevice.destroyFence( fence );
    }
    else
    {
        context->logicalDevice.destroySemaphore( semaphore );
    }
}

END_NAMESPACES
