#include "VulkanRenderPassProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< IRenderPass > VulkanRenderPassProvider::createRenderPass( const BlazarEngine::Graphics::RenderPassRequest &request )
{
    return std::make_shared< VulkanRenderPass >( context );
}

std::shared_ptr< IRenderTarget > VulkanRenderPassProvider::createRenderTarget( const RenderTargetRequest &request )
{
    auto renderTarget = std::make_shared< VulkanRenderTarget >( context );

    std::vector< vk::ImageView > attachments { };

    vk::SampleCountFlagBits msaaSampleCount = vk::SampleCountFlagBits::e1;

    if ( request.targetImages.msaa )
    {
        msaaSampleCount = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
        const vk::ImageUsageFlags &usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
        renderTarget->msaaBuffer = createAttachment( context->imageFormat, usage, vk::ImageAspectFlagBits::eColor, msaaSampleCount );
        renderTarget->hasMsaaBuffer = true;
        attachments.push_back( renderTarget->msaaBuffer.imageView );
    }

    if ( request.targetImages.depth )
    {
        vk::Format format = VulkanUtilities::findSupportedDepthFormat( context->physicalDevice );

        renderTarget->depthBuffer = createAttachment( format, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, msaaSampleCount );
        renderTarget->hasDepthBuffer = true;
        attachments.push_back( renderTarget->depthBuffer.imageView );
    }

    if ( request.type == RenderTargetType::SwapChain )
    {
        ASSERT_M ( request.outputImages.empty( ), "Cannot have output images when rendering to the swap chain image!" );

        attachments.push_back( context->swapChainImageViews[ request.frameIndex ] );
    }
    else
    {
        for ( const auto &outputImage: request.outputImages )
        {
            vk::Format vkFormat;

            if ( outputImage.imageFormat == ResourceImageFormat::R16G16B16A16Sfloat )
            {
                vkFormat = vk::Format::eR16G16B16A16Sfloat;
            }
            else if ( outputImage.imageFormat == ResourceImageFormat::R8G8B8A8Unorm )
            {
                vkFormat = vk::Format::eR8G8B8A8Unorm;
            }
            else if ( outputImage.imageFormat == ResourceImageFormat::B8G8R8A8Srgb )
            {
                vkFormat = vk::Format::eB8G8R8A8Srgb;
            }

            VulkanTextureWrapper colorAttachment = createAttachment( vkFormat,
                                                                     vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                                                                     vk::ImageAspectFlagBits::eColor,
                                                                     vk::SampleCountFlagBits::e1 );
            colorAttachment.previousUsage = ResourceUsage::RenderTarget;

            renderTarget->colorBuffers.push_back( colorAttachment );
            renderTarget->hasCustomColorAttachment = true;
            attachments.push_back( colorAttachment.imageView );

            auto &imageResource = renderTarget->outputImages.emplace_back( std::make_shared< ShaderResource >( ) );
            imageResource->type = ResourceType::Sampler2D;
            imageResource->identifier = { outputImage.outputResourceName };

            imageResource->apiSpecificBuffer = new VulkanTextureWrapper; // todo clean
            imageResource->bindStrategy = ResourceBindStrategy::BindPerFrame;
            imageResource->prepareForUsage = [ = ]( const ResourceUsage &usage )
            {
                /*auto *wrapper = ( VulkanTextureWrapper * ) imageResource->apiSpecificBuffer;
                VulkanUtilities::prepareImageForUsage( commandExecutor.get( ), wrapper, usage );*/
            };
            renderTarget->outputImageMap[ outputImage.outputResourceName ] = imageResource;

            auto * bufferRef = ( VulkanTextureWrapper * ) imageResource->apiSpecificBuffer;
            bufferRef->mipLevels = colorAttachment.mipLevels;
            bufferRef->imageView = colorAttachment.imageView;
            bufferRef->image = colorAttachment.image;
            bufferRef->sampler = colorAttachment.sampler;
            bufferRef->previousUsage = colorAttachment.previousUsage;
            bufferRef->allocation = colorAttachment.allocation;
        }
    }

    vk::FramebufferCreateInfo framebufferCreateInfo { };
    framebufferCreateInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >( request.renderPass )->getPassInstance( );
    framebufferCreateInfo.attachmentCount = attachments.size( );
    framebufferCreateInfo.pAttachments = attachments.data( );
    framebufferCreateInfo.width = context->surfaceExtent.width;
    framebufferCreateInfo.height = context->surfaceExtent.height;
    framebufferCreateInfo.layers = 1;

    renderTarget->ref = context->logicalDevice.createFramebuffer( framebufferCreateInfo );
    renderTarget->type = request.type;

    return renderTarget;
}

VulkanTextureWrapper VulkanRenderPassProvider::createAttachment( const vk::Format &format, const vk::ImageUsageFlags &usage, const vk::ImageAspectFlags &aspect,
                                                                 const vk::SampleCountFlagBits &sampleCount )
{
    VulkanTextureWrapper textureWrapper { };

    vk::ImageCreateInfo imageCreateInfo { };

    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = context->surfaceExtent.width;
    imageCreateInfo.extent.height = context->surfaceExtent.height;
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
//        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
/*        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = vk::CompareOp::eAlways;*/
        samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1.0f;

        textureWrapper.sampler = context->logicalDevice.createSampler( samplerCreateInfo );
    }

    return textureWrapper;
}

void VulkanRenderPass::create( const RenderPassRequest &request )
{
    uint32_t attachmentIndex = 0;

    // colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolve
    std::vector< vk::AttachmentDescription > attachments { };


    vk::SubpassDescription subPass { };
    subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    // Color Attachment
    vk::SampleCountFlagBits msaaSampleCount = vk::SampleCountFlagBits::e1;

    if ( request.targetImages.msaa )
    {
        propertyVal_useMsaa = "true";
        msaaSampleCount = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
    }

    {
        vk::AttachmentDescription colorAttachmentDescription { };
        colorAttachmentDescription.format = context->imageFormat;
        colorAttachmentDescription.samples = msaaSampleCount;
        colorAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;

        if ( request.isFinalDrawPass && !request.targetImages.msaa )
        {
            colorAttachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        }
        else
        {
            colorAttachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
        }

        vk::AttachmentReference colorAttachmentReference[1] { };
        colorAttachmentReference[ 0 ].attachment = attachmentIndex++;
        colorAttachmentReference[ 0 ].layout = vk::ImageLayout::eColorAttachmentOptimal;
        // --

        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments = std::move( colorAttachmentReference );

        attachments.push_back( std::move( colorAttachmentDescription ) );
    }

    if ( request.targetImages.depth )
    {
        // Depth attachment
        vk::AttachmentDescription depthAttachmentDescription { };
        depthAttachmentDescription.format = VulkanUtilities::findSupportedDepthFormat( context->physicalDevice );
        depthAttachmentDescription.samples = msaaSampleCount;
        depthAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachmentDescription.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depthAttachmentReference[1] { };
        depthAttachmentReference[ 0 ].attachment = attachmentIndex++;
        depthAttachmentReference[ 0 ].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        // --

        subPass.pDepthStencilAttachment = std::move( depthAttachmentReference );
        attachments.push_back( std::move( depthAttachmentDescription ) );
    }

    if ( request.targetImages.msaa )
    {
        // Color Image Resolver for MSAA
        vk::AttachmentDescription colorAttachmentResolve { };
        colorAttachmentResolve.format = context->imageFormat;
        colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
        colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;

        if ( request.isFinalDrawPass )
        {
            colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        }
        else
        {
            colorAttachmentResolve.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }

        vk::AttachmentReference colorAttachmentResolveReference[1] = { { } };
        colorAttachmentResolveReference[ 0 ].attachment = attachmentIndex;
        colorAttachmentResolveReference[ 0 ].layout = vk::ImageLayout::eColorAttachmentOptimal;
        // --

        subPass.pResolveAttachments = std::move( colorAttachmentResolveReference );
        attachments.push_back( std::move( colorAttachmentResolve ) );
    }


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
    else
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
}

std::string VulkanRenderPass::getProperty( const std::string &propertyName )
{
    if ( propertyName == "UseMSAA" )
    {
        return propertyVal_useMsaa;
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

    vk::ClearDepthStencilValue depthStencilClearColor = { 1.0f, 0 };

    vk::ClearValue clearValues[] = { vk::ClearColorValue { clearColor }, depthStencilClearColor };

    vk::RenderPassBeginInfo renderPassBeginInfo { };

    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = currentRenderTarget->ref;
    renderPassBeginInfo.renderArea.offset = vk::Offset2D { 0, 0 };
    renderPassBeginInfo.renderArea.extent = context->surfaceExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

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
    else if ( resource->type == ResourceType::Sampler2D || resource->type == ResourceType::CubeMap && resource->bindStrategy == ResourceBindStrategy::BindPerFrame )
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
    else if ( resource->type == ResourceType::Sampler2D || resource->type == ResourceType::CubeMap && resource->bindStrategy == ResourceBindStrategy::BindPerObject )
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

void VulkanRenderPass::draw( )
{
    FUNCTION_BREAK( vertexDataAttachment == nullptr )

    auto descriptorSets = boundPipeline->descriptorManager->getOrderedSets( frameIndex, boundPipeline->descriptorManager->getObjectCount( ) );

    buffers[ frameIndex ].bindPipeline( getBoundPipelineBindPoint( ), boundPipeline->pipeline );

    bindVertexBuffer( );
    if ( indexDataAttachment != nullptr )
    {
        bindIndexBuffer( );
    }

    buffers[ frameIndex ].setViewport( 0, context->viewport );
    buffers[ frameIndex ].setScissor( 0, context->viewScissor );

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
                1,
                0,
                0,
                0
        );
    }
    else
    {
        buffers[ frameIndex ].draw(
                vertexDataAttachment->vertexCount,
                1,
                0,
                0
        );
    }

    boundPipeline->descriptorManager->incrementObjectCounter( );
    vertexDataAttachment = nullptr;
    indexDataAttachment = nullptr;
}

void VulkanRenderPass::submit( std::vector< std::shared_ptr< IResourceLock > > waitOnLock, std::shared_ptr< IResourceLock > notifyFence )
{
    buffers[ frameIndex ].endRenderPass( );
    buffers[ frameIndex ].end( );

    if ( currentRenderTarget->type == RenderTargetType::SwapChain )
    {
        auto result = context->logicalDevice.acquireNextImageKHR( context->swapChain, UINT64_MAX, swapChainImageAvailable[ frameIndex ]->getVkSemaphore( ), nullptr );

        if ( result.result == vk::Result::eErrorOutOfDateKHR )
        {
            Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
            return;
        }
        else if ( result.result != vk::Result::eSuccess && result.result == vk::Result::eSuboptimalKHR )
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

VulkanRenderPass::~VulkanRenderPass( )
{
    context->logicalDevice.destroyRenderPass( renderPass );
}

VulkanRenderTarget::~VulkanRenderTarget( )
{
    if ( hasDepthBuffer )
    {
        context->logicalDevice.destroyImageView( depthBuffer.imageView );
        context->vma.destroyImage( depthBuffer.image, depthBuffer.allocation );
    }

    if ( hasMsaaBuffer )
    {
        context->logicalDevice.destroyImageView( msaaBuffer.imageView );
        context->vma.destroyImage( msaaBuffer.image, msaaBuffer.allocation );
    }

    if ( hasCustomColorAttachment )
    {
        for ( const auto &colorBuffer: colorBuffers )
        {
            context->logicalDevice.destroyImageView( colorBuffer.imageView );
            context->logicalDevice.destroySampler( colorBuffer.sampler );
            context->vma.destroyImage( colorBuffer.image, colorBuffer.allocation );
        }
    }

    context->logicalDevice.destroyFramebuffer( ref );
}

END_NAMESPACES