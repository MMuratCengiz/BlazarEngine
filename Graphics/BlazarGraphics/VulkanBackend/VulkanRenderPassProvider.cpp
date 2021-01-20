#include "VulkanRenderPassProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< IRenderPass > VulkanRenderPassProvider::createRenderPass( const BlazarEngine::Graphics::RenderPassRequest &request )
{
    return std::make_shared< VulkanRenderPass >( context );
}

std::shared_ptr< IRenderTarget > VulkanRenderPassProvider::createRenderTarget( const RenderTargetRequest &request )
{
    auto renderTarget = std::make_shared< VulkanRenderTarget >( context );

    renderTarget->recreateBuffer = [ = ]( )
    {
        std::vector< vk::ImageView > attachments { };

        for ( const auto &outputImage: request.outputImages )
        {
            if ( outputImage.flags.presentedImage )
            {
                attachments.push_back( context->swapChainImageViews[ request.frameIndex ] );
                continue;
            }

            auto msaaSampleCount = getOutputImageSamples( context, outputImage, true );
            auto vkFormat = getOutputImageVkFormat( context, outputImage );
            auto usageFlags = getOutputImageVkUsage( context, outputImage );
            auto aspectFlags = getOutputImageVkAspect( context, outputImage );

            auto attachment = createAttachment( vkFormat, usageFlags, aspectFlags, msaaSampleCount, request );

            if ( outputImage.attachmentType == ResourceAttachmentType::Color || outputImage.attachmentType == ResourceAttachmentType::Depth )
            {
                if ( outputImage.flags.msaaSampled && outputImage.attachmentType == ResourceAttachmentType::Color )
                {
                    msaaSampleCount = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
                    usageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;

                    auto msaaAttachment = createAttachment( vkFormat, usageFlags, aspectFlags, msaaSampleCount, request );

                    attachments.push_back( msaaAttachment.imageView );
                    renderTarget->buffers.push_back( msaaAttachment );
                }

                ResourceType type = ResourceType::Sampler2D;

                if ( outputImage.attachmentType == ResourceAttachmentType::Depth )
                {
                    type = ResourceType::DepthImage;
                }

                auto &imageResource = renderTarget->outputImages.emplace_back( std::make_shared< ShaderResource >( ) );
                imageResource->type = type;
                imageResource->identifier = { outputImage.outputResourceName };
                imageResource->apiSpecificBuffer = new VulkanTextureWrapper; // todo clean
                imageResource->bindStrategy = ResourceBindStrategy::BindPerFrame;
                imageResource->prepareForUsage = [ = ]( const ResourceUsage &usage )
                { };

                renderTarget->outputImageMap[ outputImage.outputResourceName ] = imageResource;

                auto *bufferRef = ( VulkanTextureWrapper * ) imageResource->apiSpecificBuffer;
                bufferRef->mipLevels = attachment.mipLevels;
                bufferRef->imageView = attachment.imageView;
                bufferRef->image = attachment.image;
                bufferRef->sampler = attachment.sampler;
                bufferRef->previousUsage = attachment.previousUsage;
                bufferRef->allocation = attachment.allocation;
            }

            renderTarget->buffers.push_back( attachment );
            attachments.push_back( attachment.imageView );
        }

        vk::FramebufferCreateInfo framebufferCreateInfo { };
        framebufferCreateInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >( request.renderPass )->getPassInstance( );
        framebufferCreateInfo.attachmentCount = attachments.size( );
        framebufferCreateInfo.pAttachments = attachments.data( );
        framebufferCreateInfo.width = request.renderArea.width == 0 ? context->surfaceExtent.width : request.renderArea.width;
        framebufferCreateInfo.height = request.renderArea.height == 0 ? context->surfaceExtent.height : request.renderArea.height;
        framebufferCreateInfo.layers = 1;

        renderTarget->ref = context->logicalDevice.createFramebuffer( framebufferCreateInfo );
        renderTarget->type = request.type;
    };

    renderTarget->recreateBuffer( );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::SwapChainInvalidated, [ = ]( const Input::EventType &type, std::shared_ptr< Input::IEventParameters > )
    {
        context->logicalDevice.waitIdle( );
        if ( context->surfaceExtent.width > 0 && context->surfaceExtent.height > 0 )
        {
            renderTarget->cleanup( );
            renderTarget->recreateBuffer( );
            context->logicalDevice.waitIdle( );
        }
    } );

    return renderTarget;
}

vk::ImageAspectFlags VulkanRenderPassProvider::getOutputImageVkAspect( VulkanContext *context, const OutputImage &outputImage )
{
    vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor;

    if ( outputImage.attachmentType == ResourceAttachmentType::Depth )
    {
        aspectFlags = vk::ImageAspectFlagBits::eDepth;
    }
    else if ( outputImage.attachmentType == ResourceAttachmentType::DepthAndStencil )
    {
        aspectFlags = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }
    return aspectFlags;
}

vk::ImageLayout VulkanRenderPassProvider::getOutputImageVkLayout( VulkanContext *context, const OutputImage &outputImage )
{
    vk::ImageLayout layout = vk::ImageLayout::eColorAttachmentOptimal;

    if ( outputImage.attachmentType == ResourceAttachmentType::Depth )
    {
        layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }

    if ( outputImage.attachmentType == ResourceAttachmentType::Stencil )
    {
        layout = vk::ImageLayout::eStencilAttachmentOptimal;
    }

    if ( outputImage.attachmentType == ResourceAttachmentType::DepthAndStencil )
    {
        layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }

    return layout;
}

vk::ImageUsageFlags VulkanRenderPassProvider::getOutputImageVkUsage( VulkanContext *context, const OutputImage &outputImage )
{
    vk::ImageUsageFlags usageFlags = vk::ImageUsageFlagBits::eColorAttachment;

    if ( outputImage.attachmentType == ResourceAttachmentType::Depth || outputImage.attachmentType == ResourceAttachmentType::DepthAndStencil )
    {
        usageFlags = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }

    if ( outputImage.attachmentType == ResourceAttachmentType::Color || outputImage.attachmentType == ResourceAttachmentType::Depth && !outputImage.flags.presentedImage )
    {
        usageFlags |= vk::ImageUsageFlagBits::eSampled;
    }

    return usageFlags;
}

vk::SampleCountFlagBits VulkanRenderPassProvider::getOutputImageSamples( VulkanContext *context, const OutputImage &outputImage, bool force1ForColorAttachment )
{
    vk::SampleCountFlagBits msaaSampleCount = vk::SampleCountFlagBits::e1;

    if ( force1ForColorAttachment && outputImage.attachmentType == ResourceAttachmentType::Color )
    {
        return msaaSampleCount;
    }

    if ( outputImage.flags.msaaSampled ) // Color attachments should always have 1
    {
        msaaSampleCount = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
    }

    return msaaSampleCount;
}

vk::Format VulkanRenderPassProvider::getOutputImageVkFormat( VulkanContext *context, const OutputImage &outputImage )
{
    vk::Format vkFormat;

    if ( outputImage.imageFormat == ResourceImageFormat::R16G16B16A16Sfloat )
    {
        vkFormat = vk::Format::eR16G16B16A16Sfloat;
    }
    else if ( outputImage.imageFormat == ResourceImageFormat::R32G32B32A32Sfloat )
    {
        vkFormat = vk::Format::eR32G32B32A32Sfloat;
    }
    else if ( outputImage.imageFormat == ResourceImageFormat::R8G8B8A8Unorm )
    {
        vkFormat = vk::Format::eR8G8B8A8Unorm;
    }
    else if ( outputImage.imageFormat == ResourceImageFormat::B8G8R8A8Srgb )
    {
        vkFormat = vk::Format::eB8G8R8A8Srgb;
    }
    else if ( outputImage.imageFormat == ResourceImageFormat::MatchSwapChainImageFormat )
    {
        vkFormat = context->imageFormat;
    }
    else if ( outputImage.imageFormat == ResourceImageFormat::BestDepthFormat )
    {
        vkFormat = VulkanUtilities::findSupportedDepthFormat( context->physicalDevice );
    }
    else if ( outputImage.imageFormat == ResourceImageFormat::D32Sfloat )
    {
        vkFormat = vk::Format::eD32Sfloat;
    }

    return vkFormat;
}

VulkanTextureWrapper VulkanRenderPassProvider::createAttachment( const vk::Format &format, const vk::ImageUsageFlags &usage, const vk::ImageAspectFlags &aspect,
                                                                 const vk::SampleCountFlagBits &sampleCount, const RenderTargetRequest request )
{
    VulkanTextureWrapper textureWrapper { };

    vk::ImageCreateInfo imageCreateInfo { };

    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = request.renderArea.width == 0 ? context->surfaceExtent.width : request.renderArea.width;
    imageCreateInfo.extent.height = request.renderArea.height == 0 ? context->surfaceExtent.height : request.renderArea.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.usage = usage;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = sampleCount;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;;

    vma::AllocationCreateInfo allocationCreateInfo { };
    allocationCreateInfo.usage = vma::MemoryUsage::eGpuOnly;

    auto imageAllocationPair = context->vma.createImage( imageCreateInfo, allocationCreateInfo );
    textureWrapper.image = imageAllocationPair.first;
    textureWrapper.allocation = imageAllocationPair.second;

    VulkanUtilities::createImageView( context, textureWrapper.imageView, textureWrapper.image, format, aspect );

    if ( usage & vk::ImageUsageFlagBits::eSampled )
    {
        vk::SamplerCreateInfo samplerCreateInfo { };

        samplerCreateInfo.magFilter = vk::Filter::eNearest;
        samplerCreateInfo.minFilter = vk::Filter::eNearest;
        samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
        samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
        samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
        samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1.0f;

        textureWrapper.sampler = context->logicalDevice.createSampler( samplerCreateInfo );
    }

    textureWrapper.previousUsage = ResourceUsage::RenderTarget;

    return textureWrapper;
}

void VulkanRenderPass::create( const RenderPassRequest &request )
{
    uint32_t attachmentIndex = 0;

    std::vector< vk::AttachmentDescription > attachments { };
    std::vector< vk::AttachmentReference > colorAttachments;
    std::vector< vk::AttachmentReference > resolveAttachments;
    std::vector< vk::AttachmentReference > depthAttachments;

    auto initAttachmentDefaults = [ ]( vk::AttachmentDescription &attachmentDescription )
    {
        attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
        attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
        attachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    };

    auto setAttachmentFinalLayout = [ ]( vk::AttachmentDescription &attachmentDescription, const OutputImage &outputImage )
    {
        if ( outputImage.attachmentType == ResourceAttachmentType::Depth )
        {
            if ( outputImage.flags.presentedImage )
            {
                attachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            }
            else
            {
                attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            }
        }
        else if ( outputImage.flags.presentedImage && !outputImage.flags.msaaSampled )
        {
            attachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        }
        else if ( !outputImage.flags.presentedImage && !outputImage.flags.msaaSampled )
        {
            attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else
        {
            attachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
        }
    };


    auto attachClearColor = [ & ]( const OutputImage &outputImage )
    {
        if ( outputImage.attachmentType == ResourceAttachmentType::Color )
        {
            std::array< float, 4 > clearColor( { 0.0f, 0.0f, 0.0f, 0.0f } );
            clearColors.push_back( vk::ClearColorValue { clearColor } );
        }
        else
        {
            clearColors.push_back( vk::ClearDepthStencilValue { 1.0f, 0 } );
        }
    };

    for ( auto &outputImage: request.outputImages )
    {
        auto &colorAttachmentDescription = attachments.emplace_back( vk::AttachmentDescription { } );

        colorAttachmentDescription.format = VulkanRenderPassProvider::getOutputImageVkFormat( context, outputImage );
        colorAttachmentDescription.samples = VulkanRenderPassProvider::getOutputImageSamples( context, outputImage );
        initAttachmentDefaults( colorAttachmentDescription );

        if ( outputImage.flags.msaaSampled )
        {
            propertyVal_useMsaa = "true";
        }

        setAttachmentFinalLayout( colorAttachmentDescription, outputImage );

        vk::AttachmentReference attachmentReference { };

        attachmentReference.attachment = attachmentIndex++;
        attachmentReference.layout = VulkanRenderPassProvider::getOutputImageVkLayout( context, outputImage );

        attachClearColor( outputImage );

        if ( outputImage.attachmentType == ResourceAttachmentType::Color )
        {
            colorAttachments.push_back( std::move( attachmentReference ) );

            if ( outputImage.flags.msaaSampled )
            {
                auto &colorAttachmentResolve = attachments.emplace_back( vk::AttachmentDescription { } );

                colorAttachmentResolve.format = VulkanRenderPassProvider::getOutputImageVkFormat( context, outputImage );
                colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
                initAttachmentDefaults( colorAttachmentResolve );

                if ( request.isFinalDrawPass )
                {
                    colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;
                }
                else
                {
                    colorAttachmentResolve.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                }

                auto &colorAttachmentResolveReference = resolveAttachments.emplace_back( vk::AttachmentReference { } );

                colorAttachmentResolveReference.attachment = attachmentIndex++;
                colorAttachmentResolveReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
                attachClearColor( outputImage );
            }
        }

        else if ( outputImage.attachmentType == ResourceAttachmentType::Depth ||
                  outputImage.attachmentType == ResourceAttachmentType::DepthAndStencil ||
                  outputImage.attachmentType == ResourceAttachmentType::Stencil )
        {
            depthAttachments.push_back( std::move( attachmentReference ) );
        }
    }

    std::stringstream attCountBuilder;
    attCountBuilder << colorAttachments.size( );
    propertyVal_attachmentCount = attCountBuilder.str( );

    std::vector< vk::SubpassDependency > dependencies;

    if ( request.isFinalDrawPass )
    {
        auto &dependency1 = dependencies.emplace_back( vk::SubpassDependency { } );
        dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency1.dstSubpass = 0;

        dependency1.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency1.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        dependency1.srcAccessMask = { };
        dependency1.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    }
    else if ( request.dependencySet == DependencySet::DefaultColor )
    {
        auto &dependency1 = dependencies.emplace_back( vk::SubpassDependency { } );
        dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency1.dstSubpass = 0;

        dependency1.srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        dependency1.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        dependency1.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
        dependency1.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
        ////////////////////////////////////////////////////////////////////////////
        auto &dependency2 = dependencies.emplace_back( vk::SubpassDependency { } );
        dependency2.srcSubpass = 0;
        dependency2.dstSubpass = VK_SUBPASS_EXTERNAL;

        dependency2.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency2.dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;

        dependency2.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
        dependency2.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    }
    else if ( request.dependencySet == DependencySet::ShadowMap )
    {
        auto &dependency1 = dependencies.emplace_back( vk::SubpassDependency { } );
        dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency1.dstSubpass = 0;

        dependency1.srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
        dependency1.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;

        dependency1.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        dependency1.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        dependency1.dependencyFlags = vk::DependencyFlagBits::eByRegion;
        ////////////////////////////////////////////////////////////////////////////
        auto &dependency2 = dependencies.emplace_back( vk::SubpassDependency { } );
        dependency2.srcSubpass = 0;
        dependency2.dstSubpass = VK_SUBPASS_EXTERNAL;

        dependency2.srcStageMask = vk::PipelineStageFlagBits::eLateFragmentTests;
        dependency2.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

        dependency2.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        dependency2.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        dependency2.dependencyFlags = vk::DependencyFlagBits::eByRegion;
    }

    vk::SubpassDescription subPass { };
    subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subPass.colorAttachmentCount = colorAttachments.size( );
    subPass.pColorAttachments = colorAttachments.data( );
    subPass.pDepthStencilAttachment = depthAttachments.empty( ) ? nullptr : depthAttachments.data( );
    subPass.pResolveAttachments = resolveAttachments.empty( ) ? nullptr : resolveAttachments.data( );

    vk::RenderPassCreateInfo renderPassCreateInfo { };
    renderPassCreateInfo.attachmentCount = attachments.size( );
    renderPassCreateInfo.pAttachments = attachments.data( );
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subPass;
    renderPassCreateInfo.dependencyCount = dependencies.size( );
    renderPassCreateInfo.pDependencies = dependencies.data( );

    renderPass = context->logicalDevice.createRenderPass( renderPassCreateInfo );

    vk::CommandBufferAllocateInfo bufferAllocateInfo { };
    bufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    bufferAllocateInfo.commandPool = context->graphicsQueueCommandPool;
    bufferAllocateInfo.commandBufferCount = context->swapChainImages.size( );

    buffers = context->logicalDevice.allocateCommandBuffers( bufferAllocateInfo );

    setDepthBias = request.setDepthBias;
    depthBiasConstant = request.depthBiasConstant;
    depthBiasSlope = request.depthBiasSlope;
    renderArea = request.renderArea;

    renderArea.width = renderArea.width == 0 ? context->surfaceExtent.width : renderArea.width;
    renderArea.height = renderArea.height == 0 ? context->surfaceExtent.height : renderArea.height;

    updateViewport( renderArea.width, renderArea.height );

    // Only update renderArea which want to match screen size
    if ( request.renderArea.width == 0 )
    {
        Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::WindowResized, [ & ]( const Input::EventType &eventType, std::shared_ptr< Input::IEventParameters > eventParams )
        {
            auto parameters = Input::GlobalEventHandler::ToWindowResizedParameters( eventParams );
            this->context->logicalDevice.waitIdle( );
            updateViewport( parameters->width, parameters->height );
            this->context->logicalDevice.waitIdle( );
        } );
    }
}

std::string VulkanRenderPass::getProperty( const std::string &propertyName )
{
    if ( propertyName == "UseMSAA" )
    {
        return propertyVal_useMsaa;
    }

    if ( propertyName == "AttachmentCount" )
    {
        return propertyVal_attachmentCount;
    }
    if ( propertyName == "DepthBiasEnabled" )
    {
        return setDepthBias ? "true" : "false";
    }

    return "";
}

void VulkanRenderPass::frameStart( const uint32_t &frameIndex, const std::vector< std::shared_ptr< IPipeline > > &pipelines )
{
    this->frameIndex = frameIndex;

    for ( auto &pipeline: pipelines )
    {
        auto vkPipeline = std::dynamic_pointer_cast< VulkanPipeline >( pipeline );
        vkPipeline->descriptorManager->resetObjectCounter( );
    }
}

void VulkanRenderPass::begin( std::shared_ptr< IRenderTarget > renderTarget, std::array< float, 4 > clearColor )
{
    // Todo these should match with the amount of attachments of the render target
    ASSERT_M( renderTarget != nullptr, "RenderPassRequest must pass a valid renderTarget pointer." );
    currentRenderTarget = std::dynamic_pointer_cast< VulkanRenderTarget >( renderTarget );

    vk::RenderPassBeginInfo renderPassBeginInfo { };

    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = currentRenderTarget->ref;
    renderPassBeginInfo.renderArea.offset = vk::Offset2D { renderArea.x, renderArea.y };
    renderPassBeginInfo.renderArea.extent = vk::Extent2D { renderArea.width, renderArea.height };
    renderPassBeginInfo.clearValueCount = clearColors.size( );
    renderPassBeginInfo.pClearValues = clearColors.data( );

    vk::CommandBufferBeginInfo beginInfo { };
    beginInfo.flags = { };

    buffers[ frameIndex ].begin( beginInfo );
    buffers[ frameIndex ].beginRenderPass( &renderPassBeginInfo, vk::SubpassContents::eInline );
}

void VulkanRenderPass::bindPipeline( std::shared_ptr< IPipeline > pipeline )
{
    auto vkPipeline = std::dynamic_pointer_cast< VulkanPipeline >( pipeline );
    boundPipeline = std::move( vkPipeline );
}

vk::PipelineBindPoint VulkanRenderPass::getBoundPipelineBindPoint( ) const
{
    vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics;

    if ( boundPipeline->bindPoint == BindPoint::Compute )
    {
        bindPoint = vk::PipelineBindPoint::eCompute;
    }

    return bindPoint;
}

void VulkanRenderPass::bindPerFrame( std::shared_ptr< ShaderResource > resource )
{
    if ( resource->type == ResourceType::Uniform && resource->bindStrategy == ResourceBindStrategy::BindPerFrame )
    {
        boundPipeline->descriptorManager->updateUniform(
                frameIndex,
                resource->identifier.name,
                ( ( VulkanBufferWrapper * ) resource->apiSpecificBuffer )->buffer,
                0, // Global resource object index doesn't matter
                resource->identifier.deviation
        );
    }
    else if ( resource->type == ResourceType::Sampler2D || resource->type == ResourceType::DepthImage || resource->type == ResourceType::CubeMap && resource->bindStrategy == ResourceBindStrategy::BindPerFrame )
    {
        boundPipeline->descriptorManager->updateTexture(
                frameIndex,
                resource->identifier.getKey( ),
                *( ( VulkanTextureWrapper * ) resource->apiSpecificBuffer ),
                0 // Global resource object index doesn't matter
        );
    }
}

void VulkanRenderPass::bindPerObject( std::shared_ptr< ShaderResource > resource )
{
    if ( resource->type == ResourceType::VertexData )
    {
        vertexDataAttachment = std::dynamic_pointer_cast< VertexData >( resource->dataAttachment );

        auto bufferWrapper = reinterpret_cast< VulkanBufferWrapper * >( resource->apiSpecificBuffer );

        vk::DeviceSize offset = 0;

        bindVertexBuffer = [ = ]( )
        {
            buffers[ frameIndex ].bindVertexBuffers(
                    0,
                    1,
                    &bufferWrapper->buffer.first,
                    &offset
            );
        };
    }
    else if ( resource->type == ResourceType::IndexData )
    {
        indexDataAttachment = std::dynamic_pointer_cast< IndexData >( resource->dataAttachment );

        auto bufferWrapper = reinterpret_cast< VulkanBufferWrapper * >( resource->apiSpecificBuffer );

        vk::DeviceSize offset = 0;

        bindIndexBuffer = [ = ]( )
        {
            buffers[ frameIndex ].bindIndexBuffer(
                    bufferWrapper->buffer.first,
                    offset,
                    vk::IndexType::eUint32
            );
        };
    }
    else if ( resource->type == ResourceType::PushConstant )
    {
        boundPipeline->descriptorManager->updatePushConstant(
                frameIndex,
                resource->identifier.name,
                resource->dataAttachment->content
        );
    }
    else if ( resource->type == ResourceType::Uniform && resource->bindStrategy == ResourceBindStrategy::BindPerObject )
    {
        boundPipeline->descriptorManager->updateUniform(
                frameIndex,
                resource->identifier.name,
                ( ( VulkanBufferWrapper * ) resource->apiSpecificBuffer )->buffer,
                boundPipeline->descriptorManager->getObjectCount( ),
                resource->identifier.deviation
        );
    }
    else if ( resource->type == ResourceType::Sampler2D || resource->type == ResourceType::DepthImage || resource->type == ResourceType::CubeMap && resource->bindStrategy == ResourceBindStrategy::BindPerObject )
    {
        boundPipeline->descriptorManager->updateTexture(
                frameIndex,
                resource->identifier.getKey( ),
                *( ( VulkanTextureWrapper * ) resource->apiSpecificBuffer ),
                boundPipeline->descriptorManager->getObjectCount( )
        );
    }
}

/* Type of Resources:
 * - InstanceGeometryResource
 * - GeometryResource
 * - Sampler2DResource
 * - Sampler3DResource
 * -
 */

void VulkanRenderPass::draw( const uint32_t &instanceCount )
{
    FUNCTION_BREAK( vertexDataAttachment == nullptr )

    auto descriptorSets = boundPipeline->descriptorManager->getOrderedSets( frameIndex, boundPipeline->descriptorManager->getObjectCount( ) );

    buffers[ frameIndex ].setViewport( 0, 1, &viewport );
    buffers[ frameIndex ].setScissor( 0, 1, &viewScissor );
    buffers[ frameIndex ].bindPipeline( getBoundPipelineBindPoint( ), boundPipeline->pipeline );

    bindVertexBuffer( );

    if ( indexDataAttachment != nullptr )
    {
        bindIndexBuffer( );
    }

    if ( setDepthBias )
    {
        buffers[ frameIndex ].setDepthBias( depthBiasConstant, 0.0f, depthBiasSlope );
    }

    buffers[ frameIndex ].bindDescriptorSets(
            getBoundPipelineBindPoint( ),
            boundPipeline->layout,
            0,
            descriptorSets.size( ),
            descriptorSets.data( ),
            0,
            nullptr
    );

    for ( const auto &pushConstantBinding: boundPipeline->descriptorManager->getPushConstantBindings( frameIndex ) )
    {
        buffers[ frameIndex ].pushConstants(
                boundPipeline->layout,
                pushConstantBinding.stage,
                0,
                pushConstantBinding.totalSize,
                pushConstantBinding.data );
    }

    if ( indexDataAttachment != nullptr )
    {
        buffers[ frameIndex ].drawIndexed(
                indexDataAttachment->indexCount,
                instanceCount,
                0,
                0,
                0
        );
    }
    else
    {
        buffers[ frameIndex ].draw(
                vertexDataAttachment->vertexCount,
                instanceCount,
                0,
                0
        );
    }

    boundPipeline->descriptorManager->incrementObjectCounter( );
    vertexDataAttachment = nullptr;
    indexDataAttachment = nullptr;
}

bool VulkanRenderPass::submit( std::vector< std::shared_ptr< IResourceLock > > waitOnLock, std::shared_ptr< IResourceLock > notifyFence )
{
    buffers[ frameIndex ].endRenderPass( );
    buffers[ frameIndex ].end( );

    if ( currentRenderTarget->type == RenderTargetType::SwapChain )
    {
        auto result = context->logicalDevice.acquireNextImageKHR( context->swapChain, UINT64_MAX, swapChainImageAvailable[ frameIndex ]->getVkSemaphore( ), nullptr );

        if ( result.result == vk::Result::eErrorOutOfDateKHR )
        {
            Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
            return false;
        }
        else if ( result.result != vk::Result::eSuccess && result.result != vk::Result::eSuboptimalKHR )
        {
            throw std::runtime_error( "failed to acquire swap chain image!" );
        }

        swapChainIndex = result.value;
    }

    vk::SubmitInfo submitInfo { };

    std::vector< vk::Semaphore > semaphores;

    for ( auto &waitOn: waitOnLock )
    {
        semaphores.push_back( std::dynamic_pointer_cast< VulkanResourceLock >( waitOn )->getVkSemaphore( ) );
    }

    if ( currentRenderTarget->type == RenderTargetType::SwapChain )
    {
        semaphores.push_back( swapChainImageAvailable[ frameIndex ]->getVkSemaphore( ) );
    }

    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    submitInfo.waitSemaphoreCount = semaphores.size( );
    submitInfo.pWaitSemaphores = semaphores.data( );
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffers[ frameIndex ];

    if ( currentRenderTarget->type == RenderTargetType::SwapChain )
    {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &swapChainImageRendered[ frameIndex ]->getVkSemaphore( );
    }
    else
    {
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;
    }

    notifyFence->reset( );
    auto submitResult = context->queues[ QueueType::Graphics ].submit( 1, &submitInfo, std::dynamic_pointer_cast< VulkanResourceLock >( notifyFence )->getVkFence( ) );

    VkCheckResult( submitResult );
    if ( currentRenderTarget->type == RenderTargetType::SwapChain )
    {
        presentPassToSwapChain( );
    }

    return true;
}

const vk::RenderPass &VulkanRenderPass::getPassInstance( ) const
{
    return renderPass;
}

void VulkanRenderPass::presentPassToSwapChain( )
{
    vk::PresentInfoKHR presentInfo { };

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &swapChainImageRendered[ frameIndex ]->getVkSemaphore( );
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context->swapChain;
    presentInfo.pImageIndices = &swapChainIndex;
    presentInfo.pResults = nullptr;

    auto presentResult = context->queues[ QueueType::Presentation ].presentKHR( presentInfo );

    if ( presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR )
    {
        Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
        return;
    }
    else if ( presentResult != vk::Result::eSuccess )
    {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }
}

void VulkanRenderPass::updateViewport( const uint32_t &width, const uint32_t &height )
{
    FUNCTION_BREAK( width == 0 || height == 0 )

    viewport.x = renderArea.x;
    viewport.y = renderArea.y;
    viewport.width = width;
    viewport.height = ( ( float ) height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    viewScissor.offset = vk::Offset2D( viewport.x, renderArea.y );
    viewScissor.extent = vk::Extent2D( width, height );

    renderArea.width = width;
    renderArea.height = height;
}

VulkanRenderPass::~VulkanRenderPass( )
{
    VulkanRenderPass::cleanup( );
}

void VulkanRenderPass::cleanup( )
{
    for ( auto &lock: swapChainImageAvailable )
    {
        lock->cleanup( );
    }

    for ( auto &lock: swapChainImageRendered )
    {
        lock->cleanup( );
    }

    context->logicalDevice.destroyRenderPass( renderPass );
}

VulkanRenderTarget::~VulkanRenderTarget( )
{
    VulkanRenderTarget::cleanup( );
}

void VulkanRenderTarget::cleanup( )
{
    for ( auto &buffer: buffers )
    {
        context->logicalDevice.destroyImageView( buffer.imageView );
        context->logicalDevice.destroySampler( buffer.sampler );
        context->vma.destroyImage( buffer.image, buffer.allocation );
    }

    buffers.clear( );

    context->logicalDevice.destroyFramebuffer( ref );
}

END_NAMESPACES