#pragma once

#include "../core/Common.h"
#include "InstanceContext.h"
#include "CommanExecutorArguments.h"

NAMESPACES( SomeVulkan, Graphics )

class BeginCommandExecution;
class CommandList;
class RenderDevice;
class InstanceContext;

typedef std::shared_ptr< CommandExecutor > pCommandExecutor;

class CommandExecutor {
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

class BeginCommandExecution {
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

class CommandList {
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
    CommandList *bindRenderPass( vk::PipelineBindPoint bindPoint );
    CommandList *bindVertexMemory( const vk::Buffer &vertexBuffer, const vk::DeviceSize &offset );
    CommandList *bindIndexMemory( vk::Buffer indexBuffer, const vk::DeviceSize &offset );
    CommandList *bindDescriptorSet( const vk::PipelineLayout &pipelineLayout, const vk::DescriptorSet &descriptorSet );
    CommandList *drawIndexed( const std::vector< uint32_t > &indices );
    CommandList *blitImage( const ImageBlitArgs & args );
    CommandList *pipelineBarrier( const PipelineBarrierArgs& args );
    CommandList *copyBufferToImage( const CopyBufferToImageArgs& args );
    CommandList *draw( uint32_t vertexCount );
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

END_NAMESPACES