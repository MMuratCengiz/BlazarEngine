#pragma once

#include "../Core/Common.h"
#include "Renderer.h"
#include "GLSLShaderSet.h"
#include "PipelineSelector.h"
#include "../Scene/FpsCamera.h"
#include "../Input/GlobalEventHandler.h"

NAMESPACES( SomeVulkan, Graphics )

#define ENGINE_CORE_PIPELINE_BACK_CULL "EngineCorePipelineBackCull"
#define ENGINE_CORE_PIPELINE_NONE_CULL "EngineCorePipelineNoneCull"

enum class ShaderType {
    Vertex,
    Fragment
};

struct Shader {
    ShaderType type;
    std::string filename;
};

struct PipelineOptions {
    ECS::CullMode cullMode;
};

struct PipelineCreateInfos {
    // Pipeline createInfo required structures in class scope
    std::vector< vk::PipelineShaderStageCreateInfo > pipelineStageCreateInfos;
    vk::PushConstantRange pushConstantRange{};
    vk::PipelineColorBlendAttachmentState colorBlendAttachment { };
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo { };
    vk::PipelineColorBlendStateCreateInfo colorBlending { };
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo { };
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo { };
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo { };
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo { };
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo { };
    vk::PipelineVertexInputStateCreateInfo inputStateCreateInfo { };
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{ };
    vk::Rect2D viewScissor { };
    std::vector< ShaderInfo > shaders;
    std::shared_ptr< GLSLShaderSet > shaderSet;
    // --
    PipelineOptions options;
};

class RenderSurface {
private:
    const std::array< vk::DynamicState, 2 > dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eLineWidth,
    };

    static std::unordered_map< std::string, std::vector< char > > cachedShaders;
    std::shared_ptr< InstanceContext > context;

    // To be moved, maybe?
    vk::Image samplingImage;
    vk::ImageView samplingImageView;
    vk::DeviceMemory samplingMemory;
    vk::SampleCountFlagBits msaaSampleCount;
    // --

    std::shared_ptr< PipelineSelector > pipelineSelector;
    std::vector< PipelineInstance > pipelineInstances;

    PipelineSelectorFunc enginePipelineSelector;

    std::vector< vk::ShaderModule > shaderModules;
    std::shared_ptr< Renderer > renderer;
    std::shared_ptr< Scene::Camera > camera;
public:
    RenderSurface( const std::shared_ptr< InstanceContext >&, std::shared_ptr< Scene::Camera >  camera );

    std::shared_ptr< Renderer >& getSurfaceRenderer();
    ~RenderSurface( );
private:
    void createPipelines( );
    void createPipeline( const PipelineOptions& options, PipelineInstance &instance, const std::vector< ShaderInfo >& shaderInfo );
    void createSurface( );
    void updateViewport( const uint32_t& width, const uint32_t& height );
    vk::ShaderModule createShaderModule( const std::string &filename );
    static std::vector< char > readFile( const std::string &filename );
    void configureVertexInput( PipelineCreateInfos& createInfo );
    void createSwapChain( const vk::SurfaceCapabilitiesKHR& surfaceCapabilities );
    void createImageView( vk::ImageView &imageView, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags& aspectFlags );
    void createSamplingResources( );
    void configureColorBlend( PipelineCreateInfos &createInfo );
    void configureRasterization( PipelineCreateInfos &createInfo );
    void configureViewport( PipelineCreateInfos &createInfo );
    void configureMultisampling( PipelineCreateInfos &createInfo );
    void configureDynamicState( PipelineCreateInfos &createInfo );
    void createPipelineLayout( PipelineCreateInfos& createInfo, PipelineInstance& instance );
    void createRenderPass( PipelineCreateInfos& createInfo );
    void createFrameBuffers( );
    void createDepthAttachmentImages( PipelineCreateInfos& createInfo );
    void chooseExtent2D( const vk::SurfaceCapabilitiesKHR& capabilities );
    void createSwapChainImages( vk::Format format );
    void dispose();
    void createDepthImages( );
};
END_NAMESPACES