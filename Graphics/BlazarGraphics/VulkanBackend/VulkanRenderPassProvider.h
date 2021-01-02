#pragma once

#include <BlazarCore/Common.h>
#include "VulkanContext.h"
#include "VulkanUtilities.h"
#include "VulkanPipelineProvider.h"
#include "VulkanResourceProvider.h"
#include <boost/thread.hpp>
#include "../IRenderPassProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class VulkanPipeline;

struct VulkanRenderTarget : public IRenderTarget
{
private:
    VulkanContext *context;
public:
    inline explicit VulkanRenderTarget( VulkanContext *context ) : context( context )
    { }

    vk::Framebuffer ref;

    bool hasStencilBuffer { };
    bool hasDepthBuffer { };
    bool hasCustomColorAttachment { };

    VulkanTextureWrapper stencilBuffer;
    VulkanTextureWrapper depthBuffer;
    std::vector< VulkanTextureWrapper > colorBuffers;

    ~VulkanRenderTarget( ) override;
};

class VulkanRenderPass : public IRenderPass
{
private:
    VulkanContext *context;
    std::shared_ptr< VulkanPipeline > boundPipeline;
    std::shared_ptr< VulkanRenderTarget > currentRenderTarget;
    std::shared_ptr< VertexData > vertexDataAttachment = nullptr;
    std::shared_ptr< IndexData > indexDataAttachment = nullptr;

    // In case we're rendering into the swapChain
    std::vector< std::unique_ptr< VulkanResourceLock > > swapChainImageAvailable;
    std::vector< std::unique_ptr< VulkanResourceLock > > swapChainImageRendered;
    uint32_t swapChainIndex { };
    uint32_t frameIndex { };
    // --

    std::function< void( ) > bindVertexBuffer;
    std::function< void( ) > bindIndexBuffer;

    std::vector< vk::CommandBuffer > buffers;
    vk::RenderPass renderPass;
public:
    explicit inline VulkanRenderPass( VulkanContext *context ) : context( context )
    {
        swapChainImageAvailable.resize( this->context->swapChainImages.size( ) );
        swapChainImageRendered.resize( this->context->swapChainImages.size( ) );

        for ( uint32_t i = 0; i < swapChainImageAvailable.size( ); ++i )
        {
            swapChainImageAvailable[ i ] = std::make_unique< VulkanResourceLock >( this->context, ResourceLockType::Semaphore );
            swapChainImageRendered[ i ] = std::make_unique< VulkanResourceLock >( this->context, ResourceLockType::Semaphore );
        }
    }

    void create( const RenderPassRequest &request ) override;
    void frameStart( const uint32_t &frameIndex, const std::vector< std::shared_ptr< IPipeline > >& pipelines ) override;
    void begin( std::shared_ptr< IRenderTarget > renderTarget, std::array< float, 4 > clearColor ) override;
    void bindPerFrame( std::shared_ptr< ShaderResource > resource ) override;
    void bindPipeline( std::shared_ptr< IPipeline > pipeline ) override;
    void bindPerObject( std::shared_ptr< ShaderResource > resource ) override;
    void draw( ) override;
    void submit( std::vector< std::shared_ptr< IResourceLock > > waitOnLock, std::shared_ptr< IResourceLock > notifyFence ) override;
    [[nodiscard]] const vk::RenderPass &getPassInstance( ) const;
    [[nodiscard]] vk::PipelineBindPoint getBoundPipelineBindPoint( ) const;
    ~VulkanRenderPass( ) override;
    void presentPassToSwapChain( );
};

class VulkanRenderPassProvider : public IRenderPassProvider
{
private:
    VulkanContext *context;
public:
    explicit inline VulkanRenderPassProvider( VulkanContext *context ) : context( context )
    { }

    std::shared_ptr< IRenderPass > createRenderPass( const RenderPassRequest &request ) override;
    std::shared_ptr< IRenderTarget > createRenderTarget( const RenderTargetRequest &request ) override;

    ~VulkanRenderPassProvider( ) override = default;
private:
    VulkanTextureWrapper createAttachment( const vk::Format &format, const vk::ImageUsageFlags &usage, const vk::ImageAspectFlags &aspect );
};

END_NAMESPACES
