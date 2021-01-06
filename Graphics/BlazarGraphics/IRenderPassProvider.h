#pragma once

#include <BlazarCore/Common.h>
#include "IPipelineProvider.h"
#include "IResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct IPipeline;

enum class RenderTargetType
{
    SwapChain, // Should automatically fetch the swap chain images.
    Intermediate // Should manually create the images pass it to the outputImages list.
};

enum class ResourceAttachmentType
{
    Depth,
    Stencil,
    DepthAndStencil,
    Color
};

struct OutputImageFlags
{
    bool msaaSampled : 1;
    bool presentedImage : 1;
};

struct OutputImage
{
    ResourceImageFormat imageFormat;
    ResourceAttachmentType attachmentType = ResourceAttachmentType::Color;
    OutputImageFlags flags;

    std::string outputResourceName;
    uint32_t width = 0; // 0 means full size
    uint32_t height = 0;
    uint32_t channels = 4;
};

struct IRenderTarget
{
    RenderTargetType type;

    std::vector< std::shared_ptr< ShaderResource > > outputImages;
    std::unordered_map< std::string, std::shared_ptr< ShaderResource > > outputImageMap;

    virtual ~IRenderTarget( ) = default;
};

struct EnabledPassAttachments
{
    bool depthAttachment : 1;
    bool msaaAttachment : 1;
};

struct RenderPassRequest
{
    std::vector< OutputImage > outputImages;
    bool isFinalDrawPass = false;
};

class IRenderPass
{
public:
    virtual void create( const RenderPassRequest& request ) = 0;
    virtual void frameStart( const uint32_t &frameIndex, const std::vector< std::shared_ptr< IPipeline> >& pipelines ) = 0;
    virtual void begin( std::shared_ptr< IRenderTarget > renderTarget, std::array< float, 4 > clearColor ) = 0;

    virtual void bindPerFrame( std::shared_ptr< ShaderResource > resource ) = 0;
    virtual void bindPipeline( std::shared_ptr< IPipeline > pipeline ) = 0;
    virtual void bindPerObject( std::shared_ptr< ShaderResource > resource ) = 0;

    virtual void draw( ) = 0;
    virtual void submit( std::vector< std::shared_ptr< IResourceLock > > waitOnLock, std::shared_ptr< IResourceLock > notifyFence ) = 0;
    virtual std::string getProperty( const std::string& propertyName ) = 0;
    virtual ~IRenderPass( ) = default;
};

struct RenderTargetRequest
{
    std::shared_ptr< IRenderPass > renderPass;
    RenderTargetType type;
    uint32_t frameIndex;

    std::vector< OutputImage > outputImages; // will result in the size of output images, resources will be created with the names in order
};

class IRenderPassProvider
{
public:
    virtual std::shared_ptr< IRenderPass > createRenderPass( const RenderPassRequest& request ) = 0;
    virtual std::shared_ptr< IRenderTarget > createRenderTarget( const RenderTargetRequest& request ) = 0;
    virtual ~IRenderPassProvider( ) = default;
};

END_NAMESPACES