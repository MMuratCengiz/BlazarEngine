#pragma once

#include "../core/Common.h"
#include "Renderer.h"
#include "DefaultShaderLayout.h"

NAMESPACES( SomeVulkan, Graphics )

enum class ShaderType {
    Vertex,
    Fragment
};

struct Shader {
    ShaderType type;
    std::string filename;
};

class RenderSurface {
private:
    const vk::DynamicState dynamicStates[2] = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eLineWidth,
    };

    static std::unordered_map< std::string, std::vector< char > > cachedShaders;
    std::shared_ptr< InstanceContext > context;
    std::vector< Shader > shaders;

    // Todo see if we can make these all local again
    // Pipeline createInfo required structures in class scope
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
    // --

    // To be moved, maybe?
    vk::Image samplingImage;
    vk::ImageView samplingImageView;
    vk::DeviceMemory samplingMemory;
    vk::SampleCountFlagBits msaaSampleCount;
    // --

    std::vector< vk::PipelineShaderStageCreateInfo > pipelineStageCreateInfos;

    std::vector< vk::ShaderModule > shaderModules;
    std::shared_ptr< Renderer > renderer;
    std::shared_ptr< DefaultShaderLayout > shaderLayout = std::make_shared< DefaultShaderLayout >();
public:
    RenderSurface( const std::shared_ptr< InstanceContext >&, std::vector< Shader > shaders );

    std::shared_ptr< Renderer >& getSurfaceRenderer();
    ~RenderSurface( );
private:
    void createPipeline( bool isReset  );
    void createSurface( );

    vk::ShaderModule createShaderModule( const std::string &filename );

    static std::vector< char > readFile( const std::string &filename );

    void configureVertexInput( );

    void createSwapChain( const vk::SurfaceCapabilitiesKHR& surfaceCapabilities );

    void createImageView( vk::ImageView &imageView, const vk::Image& image, const vk::Format& format,
                          const vk::ImageAspectFlags& aspectFlags );
    void createSamplingResources( );
    void configureColorBlend( );
    void configureRasterization( );
    void configureViewport( );
    void configureMultisampling( );
    void configureDynamicState( );
    void createPipelineLayout( );
    void createRenderPass( );
    void createFrameBuffers( );
    void createDepthAttachmentImages( );
    void createDescriptorPool( );
    void chooseExtent2D( const vk::SurfaceCapabilitiesKHR& capabilities );
    void createSwapChainImages( vk::Format format );
    void dispose();
};
END_NAMESPACES