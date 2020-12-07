#pragma once

#include "../Core/Common.h"
#include "InstanceContext.h"
#include "CommanExecutorArguments.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class BeginCommandExecution;
class CommandList;
class RenderDevice;
class InstanceContext;

class CommandExecutor
{
private:
    friend class BeginCommandExecution;
    friend class CommandList;

    vk::CommandPool commandPool { };
    std::shared_ptr< InstanceContext > context;

public:
    explicit CommandExecutor( const std::shared_ptr< InstanceContext > &context );

    std::shared_ptr< BeginCommandExecution > startCommandExecution( );

    ~CommandExecutor( );
};

class BeginCommandExecution
{
private:
    CommandExecutor *executor;

    std::vector< vk::CommandBuffer > buffers;
private:
    friend class CommandExecutor;
    friend class CommandList;

    explicit BeginCommandExecution( CommandExecutor *executor );
public:
    std::shared_ptr< CommandList > generateBuffers( vk::CommandBufferUsageFlags usage, uint16_t bufferCount = 1 );
};

#define ENSURE_FILTER if ( !passesFilter() ) { return this; }

class CommandList
{
private:
    CommandExecutor *executor;
    vk::CommandBufferUsageFlags usage { };

    std::vector< vk::CommandBuffer > buffers;

    bool conditionActive = false;
    bool conditionValue = false;
    bool isElse = false;
private:
    friend class BeginCommandExecution;

    CommandList( CommandExecutor *executor, std::vector< vk::CommandBuffer > buffers, vk::CommandBufferUsageFlags usage );
public:
    CommandList *beginCommand( );
    CommandList *copyBuffer( const vk::DeviceSize &size, vk::Buffer &src, vk::Buffer &dst );
    CommandList *beginRenderPass( const vk::Framebuffer frameBuffers[], const vk::ClearColorValue &clearValue );
    CommandList *endRenderPass( );
    CommandList *pushConstant( const vk::PipelineLayout &layout, const vk::ShaderStageFlags &shaderStages, const uint32_t &size, const void *data );
    CommandList *setViewport( const vk::Viewport &viewport );
    CommandList *setCullMode( const vk::CullModeFlags &cullMode );
    CommandList *setViewScissor( const vk::Rect2D &scissor );
    CommandList *bindRenderPass( const vk::Pipeline &pipeline, const vk::PipelineBindPoint &bindPoint );
    CommandList *bindVertexMemory( const vk::Buffer &vertexBuffer, const vk::DeviceSize &offset );
    CommandList *bindVertexMemories( const std::vector< vk::Buffer > &vertexBuffers, const std::vector< vk::DeviceSize > &offsets );
    CommandList *bindIndexMemory( vk::Buffer indexBuffer, const vk::DeviceSize &offset );
    CommandList *bindDescriptorSet( const vk::PipelineLayout &pipelineLayout, const std::vector< vk::DescriptorSet > descriptorSet );
    CommandList *drawIndexed( const uint32_t &indexCount );
    CommandList *blitImage( const ImageBlitArgs &args );
    CommandList *pipelineBarrier( const PipelineBarrierArgs &args );
    CommandList *copyBufferToImage( const CopyBufferToImageArgs &args );
    CommandList *draw( const uint64_t &vertexCount );
    CommandList *filter( bool condition );
    CommandList *otherwise( );
    CommandList *endFilter( );

    vk::Result execute( );
    const std::vector< vk::CommandBuffer > &getBuffers( );
    ~CommandList( );

    void freeBuffers( );
private:
    [[nodiscard]] bool passesFilter( ) const;
};

typedef std::shared_ptr< CommandExecutor > pCommandExecutor;

END_NAMESPACES