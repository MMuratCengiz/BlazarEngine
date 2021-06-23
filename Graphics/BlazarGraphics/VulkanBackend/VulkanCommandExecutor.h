/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "VulkanContext.h"
#include "CommandExecutorArguments.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class BeginCommandExecution;
class CommandList;
class VulkanDevice;
class VulkanContext;

class VulkanCommandExecutor
{
private:
    friend class BeginCommandExecution;
    friend class CommandList;

    vk::CommandPool commandPool { };
    VulkanContext * context;

public:
    explicit VulkanCommandExecutor( VulkanContext * context );

    std::shared_ptr< BeginCommandExecution > startCommandExecution( );

    ~VulkanCommandExecutor( );
};

class BeginCommandExecution
{
private:
    VulkanCommandExecutor *executor;

    std::vector< vk::CommandBuffer > buffers;
private:
    friend class VulkanCommandExecutor;
    friend class CommandList;

    explicit BeginCommandExecution( VulkanCommandExecutor *executor );
public:
    std::shared_ptr< CommandList > generateBuffers( vk::CommandBufferUsageFlags usage, uint16_t bufferCount = 1 );
};

#define ENSURE_FILTER if ( !passesFilter() ) { return this; }

class CommandList
{
private:
    VulkanCommandExecutor *executor;
    vk::CommandBufferUsageFlags usage { };

    std::vector< vk::CommandBuffer > buffers;

    bool conditionActive = false;
    bool conditionValue = false;
    bool isElse = false;
private:
    friend class BeginCommandExecution;

    CommandList( VulkanCommandExecutor *executor, std::vector< vk::CommandBuffer > buffers, vk::CommandBufferUsageFlags usage );
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

typedef std::shared_ptr< VulkanCommandExecutor > pCommandExecutor;

END_NAMESPACES