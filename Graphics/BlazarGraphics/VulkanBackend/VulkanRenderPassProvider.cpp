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

    if ( ( request.targetImages == RenderTargetImages::Stencil ) || ( request.targetImages == RenderTargetImages::DepthAndStencil ) )
    {
        const vk::ImageUsageFlags &usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
        renderTarget->stencilBuffer = createAttachment( context->imageFormat, usage, vk::ImageAspectFlagBits::eColor );
        renderTarget->hasStencilBuffer = true;
        attachments.push_back( renderTarget->stencilBuffer.imageView );
    }

    if ( ( request.targetImages == RenderTargetImages::Depth ) || ( request.targetImages == RenderTargetImages::DepthAndStencil ) )
    {
        vk::Format format = VulkanUtilities::findSupportedDepthFormat( context->physicalDevice );

        renderTarget->depthBuffer = createAttachment( format, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth );
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

            const VulkanTextureWrapper &colorAttachment = createAttachment( vkFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eColor );
            renderTarget->colorBuffers.push_back( colorAttachment );
            renderTarget->hasDepthBuffer = true;
            attachments.push_back( colorAttachment.imageView );
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

VulkanTextureWrapper VulkanRenderPassProvider::createAttachment( const vk::Format &format, const vk::ImageUsageFlags &usage, const vk::ImageAspectFlags &aspect )
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
    imageCreateInfo.samples = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;;

    vma::AllocationCreateInfo allocationCreateInfo { };
    allocationCreateInfo.usage = vma::MemoryUsage::eGpuOnly;

    auto imageAllocationPair = context->vma.createImage( imageCreateInfo, allocationCreateInfo );
    textureWrapper.image = imageAllocationPair.first;
    textureWrapper.allocation = imageAllocationPair.second;

    VulkanUtilities::createImageView( context, textureWrapper.imageView, textureWrapper.image, format, aspect );

    return textureWrapper;
}

void VulkanRenderPass::create( const RenderPassRequest &request )
{
    // Color Attachment
    auto msaaSampleCount = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
    vk::AttachmentDescription colorAttachmentDescription { };
    colorAttachmentDescription.format = context->imageFormat;
    colorAttachmentDescription.samples = msaaSampleCount;
    colorAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference colorAttachmentReference { };
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    // --

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

    vk::AttachmentReference depthAttachmentReference { };
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    // --

    // Color Image Resolver for MSAA
    vk::AttachmentDescription colorAttachmentResolve { };
    colorAttachmentResolve.format = context->imageFormat;
    colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentResolveReference { };
    colorAttachmentResolveReference.attachment = 2;
    colorAttachmentResolveReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    // --

    vk::SubpassDescription subPass { };
    subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentReference;
    subPass.pDepthStencilAttachment = &depthAttachmentReference;
    subPass.pResolveAttachments = &colorAttachmentResolveReference;

    std::array< vk::AttachmentDescription, 3 > attachments {
            colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolve };

    vk::SubpassDependency dependency { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = { }; // TODO Recheck
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassCreateInfo { };
    renderPassCreateInfo.attachmentCount = attachments.size( );
    renderPassCreateInfo.pAttachments = attachments.data( );
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subPass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    renderPass = context->logicalDevice.createRenderPass( renderPassCreateInfo );

    vk::CommandBufferAllocateInfo bufferAllocateInfo { };
    bufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    bufferAllocateInfo.commandPool = context->graphicsQueueCommandPool;
    bufferAllocateInfo.commandBufferCount = context->swapChainImages.size( );

    buffers = context->logicalDevice.allocateCommandBuffers( bufferAllocateInfo );
}

void VulkanRenderPass::frameStart( const uint32_t &frameIndex, const std::vector< std::shared_ptr< IPipeline > >& pipelines )
{
    this->frameIndex = frameIndex;

    for ( auto& pipeline: pipelines )
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

    if ( hasStencilBuffer )
    {
        context->logicalDevice.destroyImageView( stencilBuffer.imageView );
        context->vma.destroyImage( stencilBuffer.image, stencilBuffer.allocation );
    }

    if ( hasCustomColorAttachment )
    {
        for ( const auto &colorBuffer: colorBuffers )
        {
            context->logicalDevice.destroyImageView( colorBuffer.imageView );
            context->vma.destroyImage( colorBuffer.image, colorBuffer.allocation );
        }
    }

    context->logicalDevice.destroyFramebuffer( ref );
}

END_NAMESPACES