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

#include <BlazarGraphics/GraphicsCommonIncludes.h>
#include <BlazarGraphics/IPipelineProvider.h>
#include "DescriptorManager.h"
#include "GLSLShaderSet.h"
#include "VulkanRenderPassProvider.h"
#include <BlazarCore/Common.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class VulkanRenderPass;
class VulkanRenderPassProvider;

struct VulkanPipeline : IPipeline
{
    VulkanContext *context;

    std::shared_ptr< DescriptorManager > descriptorManager;
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;
    bool alreadyDisposed = false;

    inline void cleanup( ) override
    {
        FUNCTION_BREAK( alreadyDisposed )
        alreadyDisposed = true;

        descriptorManager.reset( );
        context->logicalDevice.destroyPipeline( pipeline );
        context->logicalDevice.destroyPipelineLayout( layout );
    }

    ~VulkanPipeline( ) override
    {
        VulkanPipeline::cleanup( );
    }
};

struct PipelineCreateInfos
{
    // Pipeline createInfo required structures in class scope
    std::vector< vk::PipelineShaderStageCreateInfo > pipelineStageCreateInfos;
    std::vector< vk::PipelineColorBlendAttachmentState > colorBlendAttachments { };
    vk::PipelineTessellationStateCreateInfo tessellationStateCreateInfo { };
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo { };
    vk::PipelineColorBlendStateCreateInfo colorBlending { };
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo { };
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo { };
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo { };
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo { };
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo { };
    vk::PipelineVertexInputStateCreateInfo inputStateCreateInfo { };
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo { };
    vk::Rect2D viewScissor { };
    std::vector< GLSLShaderInfo > shaders;
    std::shared_ptr< GLSLShaderSet > shaderSet;
    // --
    PipelineRequest request;
    std::shared_ptr< VulkanRenderPass > parentPass;
};

class VulkanPipelineProvider : public IPipelineProvider
{
private:
    const std::array< vk::DynamicState, 4 > dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eDepthBias,
            vk::DynamicState::eScissor,
            vk::DynamicState::eLineWidth
    };

    VulkanContext *context;
    std::vector< std::unique_ptr< VulkanPipeline > > pipelineInstances;
    std::vector< vk::ShaderModule > shaderModules;
public:
    explicit inline VulkanPipelineProvider( VulkanContext *context ) : context( context )
    { }

    IPipeline * createPipeline( const PipelineRequest &request ) override;

    void createPipeline( const PipelineRequest &request, VulkanPipeline * pipeline, const std::vector< GLSLShaderInfo > &shaderInfo );
    void configureVertexInput( PipelineCreateInfos &createInfo );
    void configureColorBlend( PipelineCreateInfos &createInfo );
    void configureRasterization( PipelineCreateInfos &createInfo );
    void configureViewport( PipelineCreateInfos &createInfo );
    void configureMultisampling( PipelineCreateInfos &createInfo );
    void configureDynamicState( PipelineCreateInfos &createInfo );
    void createPipelineLayout( PipelineCreateInfos &createInfo, VulkanPipeline * pipeline );
    void createRenderPass( PipelineCreateInfos &createInfo );
    void createDepthAttachmentImages( PipelineCreateInfos &createInfo );
    vk::ShaderModule createShaderModule( std::vector< uint32_t > data );

    ~VulkanPipelineProvider( ) override;
};

END_NAMESPACES
