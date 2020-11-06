#pragma once

#include "../core/Common.h"
#include "Renderer.h"
#include "DefaultShaderLayout.h"


NAMESPACES( SomeVulkan, Graphics )

typedef enum class ShaderType {
    Vertex,
    Fragment
} ShaderType;

typedef struct Shader {
    ShaderType type;
    std::string filename;
} Shader;

class RenderSurface {
private:
    const VkDynamicState dynamicStates[2] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
    };

    static std::unordered_map< std::string, std::vector< char > > cachedShaders;
    std::shared_ptr< RenderContext > context;
    std::vector< Shader > shaders;

    // Pipeline createInfo required structures in class scope
    VkPipelineColorBlendAttachmentState colorBlendAttachment { };
    VkGraphicsPipelineCreateInfo pipelineCreateInfo { };
    VkPipelineColorBlendStateCreateInfo colorBlending { };
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo { };
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo { };
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo { };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo { };
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { };
    VkPipelineVertexInputStateCreateInfo inputStateCreateInfo { };
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{ };
    VkRect2D viewScissor { };
    // --

    // To be moved, maybe?
    VkImage samplingImage;
    VkImageView samplingImageView;
    VkDeviceMemory samplingMemory;
    VkSampleCountFlagBits msaaSampleCount;
    // --

    std::vector< VkPipelineShaderStageCreateInfo > pipelineStageCreateInfos;

    std::vector< VkShaderModule > shaderModules;
    std::shared_ptr< Renderer > renderer;
    std::shared_ptr< DefaultShaderLayout > shaderLayout = std::make_shared< DefaultShaderLayout >();
public:
    RenderSurface( const std::shared_ptr< RenderContext >&, std::vector< Shader > shaders );

    std::shared_ptr< Renderer >& getSurfaceRenderer();
    ~RenderSurface( );
private:
    void createPipeline( bool isReset  );
    void createSurface( );

    VkShaderModule createShaderModule( const std::string &filename );
    VkFormat findSupportedDepthFormat( );

    static std::vector< char > readFile( const std::string &filename );

    void configureVertexInput( );

    void createSwapChain( VkSurfaceCapabilitiesKHR surfaceCapabilities, VkSurfaceFormatKHR surfaceFormat,
                          VkPresentModeKHR presentMode );

    void createImageView( VkImageView &imageView, const VkImage& image, const VkFormat& format,
                          const VkImageAspectFlags& aspectFlags );
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
    void chooseExtent2D( const VkSurfaceCapabilitiesKHR& capabilities );
    void createSwapChainImages( VkFormat format );
    void dispose();
};
END_NAMESPACES