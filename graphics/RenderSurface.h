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

    VkPipelineLayout pipelineLayout;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo { };

    VkPipelineColorBlendStateCreateInfo colorBlending { };
    VkPipelineColorBlendAttachmentState colorBlendAttachment { };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo { };
    VkRect2D viewScissor { };
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo { };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo { };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo { };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { };
    VkPipelineVertexInputStateCreateInfo inputStateCreateInfo { };
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };

    std::vector< VkPipelineShaderStageCreateInfo > pipelineStageCreateInfos;
    std::vector< VkShaderModule > shaderModules;

    std::shared_ptr< Renderer > renderer;
    std::shared_ptr< DefaultShaderLayout > defaultShaderLayout = std::make_shared< DefaultShaderLayout >();
public:
    RenderSurface( const std::shared_ptr< RenderContext >&, const std::vector< Shader >& shaders );

    std::shared_ptr< Renderer >& getSurfaceRenderer();
    ~RenderSurface( );
private:
    void createSurface();

    VkShaderModule createShaderModule( const std::string &filename );
    static std::vector< char > readFile( const std::string &filename );

    void configureVertexInput( const std::vector< Shader > &shaders );

    void createSwapChain( VkSurfaceCapabilitiesKHR surfaceCapabilities, VkSurfaceFormatKHR surfaceFormat,
                          VkPresentModeKHR presentMode );
    void configureColorBlend( );
    void configureRasterization( );
    void configureViewport( );
    void configureMultisampling( );
    void configureDynamicState( );
    void createPipelineLayout( );
    void createRenderPass( );
    void createFrameBuffers( );
    void chooseExtent2D( const VkSurfaceCapabilitiesKHR& capabilities );
    void createImageAndImageViews( VkFormat format );
};
END_NAMESPACES