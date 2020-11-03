#pragma once

#include "../core/Common.h"
#include "RenderContext.h"

NAMESPACES( SomeVulkan, Graphics )

class BeginCommandExecution;
class CommandList;
class RenderDevice;

typedef std::shared_ptr< CommandExecutor > pCommandExecutor;

class CommandExecutor {
private:
    friend class BeginCommandExecution;
    friend class CommandList;

    VkCommandPool commandPool { };
    std::shared_ptr< RenderContext > context;

public:
    explicit CommandExecutor( const std::shared_ptr< RenderContext >& context );

    std::shared_ptr< BeginCommandExecution > startCommandExecution( );

    ~CommandExecutor( );
};

class BeginCommandExecution {
private:
    CommandExecutor *executor;

    std::vector< VkCommandBuffer > buffers;
private:
    friend class CommandExecutor;
    friend class CommandList;

    explicit BeginCommandExecution( CommandExecutor *executor );
public:
    std::shared_ptr< CommandList > generateBuffers( VkCommandBufferUsageFlags usage, uint16_t bufferCount = 1 );
};

#define ENSURE_FILTER if ( !passesFilter() ) { return this; }

class CommandList {
private:
    CommandExecutor *executor;
    VkCommandBufferUsageFlags usage{};

    std::vector< VkCommandBuffer > buffers;

    bool conditionActive = false;
    bool conditionValue = false;
    bool isElse = false;
private:
    friend class BeginCommandExecution;

    CommandList( CommandExecutor *executor, std::vector< VkCommandBuffer > buffers, VkCommandBufferUsageFlags usage );
public:
    CommandList *beginCommand();
    CommandList *copyBuffer( const VkDeviceSize &size, VkBuffer &src, VkBuffer &dst );
    CommandList *beginRenderPass( const VkFramebuffer frameBuffers[], const VkClearValue &clearValue );
    CommandList *endRenderPass( );
    CommandList *bindRenderPass( VkPipelineBindPoint bindPoint );
    CommandList *bindVertexMemory( const VkBuffer &vertexBuffer, const VkDeviceSize &offset );
    CommandList *bindIndexMemory( VkBuffer indexBuffer, const VkDeviceSize &offset );
    CommandList *bindDescriptorSet( const VkPipelineLayout& pipelineLayout, const VkDescriptorSet& descriptorSet );
    CommandList *drawIndexed( const std::vector< uint32_t > &indices );
    CommandList *pipelineBarrier( VkImage& image, const VkImageLayout& oldLayout, const VkImageLayout& newLayout );
    CommandList *draw( uint32_t vertexCount );
    CommandList *filter( bool condition );
    CommandList *otherwise( );
    CommandList *endFilter();

    VkResult execute( );
    const std::vector< VkCommandBuffer >& getBuffers( );
    ~CommandList();

    void freeBuffers();
private:
    [[nodiscard]] bool passesFilter( ) const;
};

END_NAMESPACES