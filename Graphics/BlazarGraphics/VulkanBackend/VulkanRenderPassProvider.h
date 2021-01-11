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
    std::function< void( ) > recreateBuffer;

    inline explicit VulkanRenderTarget( VulkanContext *context ) : context( context )
    { }

    vk::Framebuffer ref;
    std::vector< VulkanTextureWrapper > buffers;

    void cleanup( ) override;
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

    RenderArea renderArea;

    // In case we're rendering into the swapChain
    std::vector< std::unique_ptr< VulkanResourceLock > > swapChainImageAvailable;
    std::vector< std::unique_ptr< VulkanResourceLock > > swapChainImageRendered;
    std::vector< vk::ClearValue > clearColors;
    uint32_t swapChainIndex { };
    uint32_t frameIndex { };
    // --

    std::function< void( ) > bindVertexBuffer;
    std::function< void( ) > bindIndexBuffer;

    std::vector< vk::CommandBuffer > buffers;
    vk::RenderPass renderPass;

    std::string propertyVal_useMsaa = "false";
    std::string propertyVal_attachmentCount = "0";

    vk::Viewport viewport { };
    vk::Rect2D viewScissor { };

    bool setDepthBias = false;
    float depthBiasConstant;
    float depthBiasSlope;
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
    std::string getProperty( const std::string& propertyName ) override;

    [[nodiscard]] inline RenderArea getRenderArea( ) const override { return renderArea; };
    const inline vk::Viewport& getViewport( ) { return viewport; };
    const inline vk::Rect2D& getViewScissor( ) { return viewScissor; };
    void updateViewport( const uint32_t& width, const uint32_t& height );

    void draw( ) override;
    bool submit( std::vector< std::shared_ptr< IResourceLock > > waitOnLock, std::shared_ptr< IResourceLock > notifyFence ) override;
    [[nodiscard]] const vk::RenderPass &getPassInstance( ) const;
    [[nodiscard]] vk::PipelineBindPoint getBoundPipelineBindPoint( ) const;
    void presentPassToSwapChain( );

    void cleanup( ) override;
    ~VulkanRenderPass( ) override;
};

class VulkanRenderPassProvider : public IRenderPassProvider
{
private:
    VulkanContext *context;
    std::unique_ptr< VulkanCommandExecutor > commandExecutor;
public:
    explicit inline VulkanRenderPassProvider( VulkanContext *context ) : context( context )
    {
        commandExecutor = std::make_unique< VulkanCommandExecutor >( context );
    }

    std::shared_ptr< IRenderPass > createRenderPass( const RenderPassRequest &request ) override;
    std::shared_ptr< IRenderTarget > createRenderTarget( const RenderTargetRequest &request ) override;

    static vk::Format getOutputImageVkFormat( VulkanContext *context, const OutputImage &outputImage );
    static vk::SampleCountFlagBits getOutputImageSamples( VulkanContext *context, const OutputImage &outputImage, bool force1ForColorAttachment = false );
    static vk::ImageUsageFlags getOutputImageVkUsage( VulkanContext *context, const OutputImage &outputImage );
    static vk::ImageAspectFlags getOutputImageVkAspect( VulkanContext *context, const OutputImage &outputImage );
    static vk::ImageLayout getOutputImageVkLayout( VulkanContext *context, const OutputImage &outputImage );

    ~VulkanRenderPassProvider( ) override = default;
private:
    VulkanTextureWrapper createAttachment( const vk::Format &format, const vk::ImageUsageFlags &usage, const vk::ImageAspectFlags &aspect, const vk::SampleCountFlagBits &sampleCount, const RenderTargetRequest request );
};

END_NAMESPACES
