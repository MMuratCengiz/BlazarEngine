#pragma once

#include "../GraphicsCommonIncludes.h"
#include "../IPipelineProvider.h"
#include "DescriptorManager.h"
#include "GLSLShaderSet.h"
#include "VulkanRenderPassProvider.h"
#include <BlazarCore/Common.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class VulkanRenderPass;
class VulkanRenderPassProvider;

struct VulkanPipeline : IPipeline {
    VulkanContext * context;

    std::shared_ptr< DescriptorManager > descriptorManager;
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;

    ~VulkanPipeline( ) override
    {
        descriptorManager.reset( );
        context->logicalDevice.destroyPipeline( pipeline );
        context->logicalDevice.destroyPipelineLayout( layout );
    }
};

struct PipelineCreateInfos
{
    // Pipeline createInfo required structures in class scope
    std::vector< vk::PipelineShaderStageCreateInfo > pipelineStageCreateInfos;
    std::vector< vk::PipelineColorBlendAttachmentState > colorBlendAttachments { };
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
};

class VulkanPipelineProvider: public IPipelineProvider
{
private:
    const std::array< vk::DynamicState, 2 > dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eLineWidth
    };

    VulkanContext * context;
    std::vector< std::shared_ptr< VulkanPipeline > > pipelineInstances;
    std::vector< vk::ShaderModule > shaderModules;
public:
    explicit inline VulkanPipelineProvider( VulkanContext * context ) : context( context ) { }
    std::shared_ptr< IPipeline > createPipeline( const PipelineRequest &request ) override;

    void createPipeline( const PipelineRequest &request, const std::shared_ptr< VulkanPipeline > &instance, const std::vector< GLSLShaderInfo > &shaderInfo );
    void configureVertexInput( PipelineCreateInfos &createInfo );
    void configureColorBlend( PipelineCreateInfos &createInfo );
    void configureRasterization( PipelineCreateInfos &createInfo );
    void configureViewport( PipelineCreateInfos &createInfo );
    void configureMultisampling( PipelineCreateInfos &createInfo );
    void configureDynamicState( PipelineCreateInfos &createInfo );
    void createPipelineLayout( PipelineCreateInfos &createInfo, const std::shared_ptr< VulkanPipeline > &instance );
    void createRenderPass( PipelineCreateInfos &createInfo );
    void createDepthAttachmentImages( PipelineCreateInfos &createInfo );
    vk::ShaderModule createShaderModule( const std::string &filename );

    ~VulkanPipelineProvider( ) override;
};

END_NAMESPACES
