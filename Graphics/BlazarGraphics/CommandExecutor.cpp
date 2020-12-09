#include "CommandExecutor.h"

using namespace ENGINE_NAMESPACE::Graphics;

CommandExecutor::CommandExecutor( const std::shared_ptr< InstanceContext > &context ) : context( context )
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo { };

    commandPoolCreateInfo.queueFamilyIndex = context->queueFamilies[ QueueType::Graphics ].index;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    commandPool = context->logicalDevice.createCommandPool( commandPoolCreateInfo );
}

std::shared_ptr< BeginCommandExecution > CommandExecutor::startCommandExecution( )
{
    auto *pExecution = new BeginCommandExecution { this };
    return std::shared_ptr< BeginCommandExecution >( pExecution );
}

CommandExecutor::~CommandExecutor( )
{
    context->logicalDevice.destroyCommandPool( commandPool );
}

BeginCommandExecution::BeginCommandExecution( CommandExecutor *executor ) : executor( executor )
{ }

std::shared_ptr< CommandList >
BeginCommandExecution::generateBuffers( vk::CommandBufferUsageFlags usage, uint16_t bufferCount )
{
    buffers.resize( bufferCount );

    vk::CommandBufferAllocateInfo bufferAllocateInfo { };
    bufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    bufferAllocateInfo.commandPool = executor->commandPool;
    bufferAllocateInfo.commandBufferCount = bufferCount;

    buffers = executor->context->logicalDevice.allocateCommandBuffers( bufferAllocateInfo );

    auto *pCommandList = new CommandList { executor, buffers, usage };
    return std::move( std::shared_ptr< CommandList >( pCommandList ) );
}

CommandList::CommandList(
        CommandExecutor *executor,
        std::vector< vk::CommandBuffer > buffers,
        vk::CommandBufferUsageFlags usage ) : executor( executor ), buffers( std::move( buffers ) ), usage( usage )
{
}

CommandList *CommandList::beginCommand( )
{
    for ( vk::CommandBuffer buffer: buffers )
    {
        buffer.begin( usage );
    }

    return this;
}

CommandList *CommandList::copyBuffer( const vk::DeviceSize &size, vk::Buffer &src, vk::Buffer &dst )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer buffer: buffers )
    {
        vk::BufferCopy bufferCopy { };
        bufferCopy.size = size;

        buffer.copyBuffer( src, dst, 1, &bufferCopy );
    }

    return this;
}

CommandList *CommandList::beginRenderPass( const vk::Framebuffer frameBuffers[], const vk::ClearColorValue &clearValue )
{
    ENSURE_FILTER

    uint16_t index = 0;

    vk::ClearDepthStencilValue depthStencilClearColor = { 1.0f, 0 };

    vk::ClearValue clearValues[] = { clearValue, depthStencilClearColor };

    for ( vk::CommandBuffer buffer: buffers )
    {
        vk::RenderPassBeginInfo renderPassBeginInfo { };

        renderPassBeginInfo.renderPass = executor->context->renderPass;
        renderPassBeginInfo.framebuffer = frameBuffers[ index++ ];
        renderPassBeginInfo.renderArea.offset = vk::Offset2D { 0, 0 };
        renderPassBeginInfo.renderArea.extent = executor->context->surfaceExtent;
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        buffer.beginRenderPass( &renderPassBeginInfo, vk::SubpassContents::eInline );
    }

    return this;
}

CommandList *CommandList::endRenderPass( )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer buffer: buffers )
    {
        buffer.endRenderPass( );
    }

    return this;
}

CommandList *CommandList::bindRenderPass( const vk::Pipeline &pipeline, const vk::PipelineBindPoint &bindPoint )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer buffer: buffers )
    {
        buffer.bindPipeline( bindPoint, pipeline );
    }

    return this;
}

CommandList *CommandList::drawIndexed( const uint32_t &indexCount )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer buffer: buffers )
    {
        buffer.drawIndexed( indexCount, 1, 0, 0, 0 );
    }

    return this;
}

CommandList *CommandList::draw( const uint64_t &vertexCount )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer buffer: buffers )
    {
        buffer.draw( vertexCount, 1, 0, 0 );
    }

    return this;
}

CommandList *CommandList::bindVertexMemory( const vk::Buffer &vertexBuffer, const vk::DeviceSize &offset )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.bindVertexBuffers( 0, 1, &vertexBuffer, &offset );
    }

    return this;
}

CommandList *CommandList::bindVertexMemories( const std::vector< vk::Buffer > &vertexBuffers, const std::vector< vk::DeviceSize > &offsets )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.bindVertexBuffers( 0, vertexBuffers.size( ), vertexBuffers.data( ), offsets.data( ) );
    }

    return this;
}

CommandList *CommandList::bindIndexMemory( vk::Buffer indexBuffer, const vk::DeviceSize &offset )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.bindIndexBuffer( indexBuffer, offset, vk::IndexType::eUint32 );
    }

    return this;
}

CommandList *CommandList::setViewport( const vk::Viewport &viewport )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.setViewport( 0, viewport );
    }

    return this;
}

CommandList *CommandList::setCullMode( const vk::CullModeFlags &cullMode )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.setCullModeEXT( cullMode );
    }

    return this;
}

CommandList *CommandList::setViewScissor( const vk::Rect2D &scissor )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.setScissor( 0, scissor );
    }

    return this;
}

CommandList *CommandList::pushConstant( const vk::PipelineLayout &layout, const vk::ShaderStageFlags &shaderStages, const uint32_t &size, const void *data )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.pushConstants( layout, shaderStages, 0, size, data );
    }

    return this;
}

CommandList *CommandList::blitImage( const ImageBlitArgs &args )
{
    ENSURE_FILTER

    vk::ImageBlit imageBlit { };

    imageBlit.srcOffsets[ 0 ] = args.srcOffsets[ 0 ];
    imageBlit.dstOffsets[ 0 ] = args.dstOffsets[ 0 ];
    imageBlit.srcOffsets[ 1 ] = args.srcOffsets[ 1 ];
    imageBlit.dstOffsets[ 1 ] = args.dstOffsets[ 1 ];

    imageBlit.srcSubresource = args.srcSubresource;
    imageBlit.dstSubresource = args.dstSubresource;

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.blitImage(
                args.sourceImage,
                args.sourceImageLayout,
                args.destinationImage,
                args.destinationImageLayout,
                1,
                &imageBlit,
                vk::Filter::eLinear );
    }

    return this;
}

CommandList *CommandList::pipelineBarrier( const PipelineBarrierArgs &args )
{
    ENSURE_FILTER

    vk::ImageMemoryBarrier memoryBarrier { };

    memoryBarrier.oldLayout = args.oldLayout;
    memoryBarrier.newLayout = args.newLayout;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.image = args.image;
    memoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    memoryBarrier.subresourceRange.baseMipLevel = args.baseMipLevel;
    memoryBarrier.subresourceRange.levelCount = args.mipLevel;
    memoryBarrier.subresourceRange.baseArrayLayer = args.baseArrayLayer;
    memoryBarrier.subresourceRange.layerCount = args.layerCount;
    memoryBarrier.srcAccessMask = args.sourceAccess;
    memoryBarrier.dstAccessMask = args.destinationAccess;

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.pipelineBarrier(
                args.sourceStage, args.destinationStage,
                { }, 0,
                nullptr, 0,
                nullptr,
                1, &memoryBarrier );
    }

    return this;
}

CommandList *CommandList::copyBufferToImage( const CopyBufferToImageArgs &args )
{
    ENSURE_FILTER

    vk::BufferImageCopy bufferImageCopy { };

    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    bufferImageCopy.imageSubresource.mipLevel = args.mipLevel;
    bufferImageCopy.imageSubresource.baseArrayLayer = args.arrayLayer;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageOffset = vk::Offset3D { 0, 0, 0 };
    bufferImageCopy.imageExtent = vk::Extent3D {
            args.width,
            args.height,
            1
    };

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.copyBufferToImage( args.sourceBuffer, args.image, vk::ImageLayout::eTransferDstOptimal, 1,
                                  &bufferImageCopy );
    }

    return this;
}

CommandList *
CommandList::bindDescriptorSet( const vk::PipelineLayout &pipelineLayout, const std::vector< vk::DescriptorSet > descriptorSet )
{
    ENSURE_FILTER

    for ( vk::CommandBuffer &buffer: buffers )
    {
        buffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet.size( ), descriptorSet.data( ), 0, nullptr );
    }

    return this;
}

vk::Result CommandList::execute( )
{
    for ( vk::CommandBuffer buffer: buffers )
    {
        buffer.end( );
    }

    vk::SubmitInfo submitInfo { };
    submitInfo.commandBufferCount = buffers.size( );
    submitInfo.pCommandBuffers = buffers.data( );

    vk::Queue queue = executor->context->queues[ QueueType::Graphics ];
    vk::Result result = queue.submit( 1, &submitInfo, nullptr );
    queue.waitIdle( );

    return result;
}

const std::vector< vk::CommandBuffer > &CommandList::getBuffers( )
{
    return buffers;
}

CommandList *CommandList::filter( bool condition )
{
    conditionActive = true;
    conditionValue = condition;
    return this;
}

bool CommandList::passesFilter( ) const
{
    return !conditionActive || conditionValue;
}

CommandList *CommandList::otherwise( )
{
    conditionValue = !conditionValue;
    isElse = true;
    conditionActive = true;
    return this;
}

CommandList *CommandList::endFilter( )
{
    conditionActive = false;
    isElse = false;
    return this;
}

CommandList::~CommandList( )
{
    freeBuffers( );
}

void CommandList::freeBuffers( )
{
    executor->context->logicalDevice.freeCommandBuffers( executor->commandPool,
                                                         buffers.size( ),
                                                         buffers.data( ) );
}
