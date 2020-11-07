//
// Created by Murat on 10/18/2020.
//

#include <fstream>
#include "RenderSurface.h"

NAMESPACES( SomeVulkan, Graphics )

std::unordered_map< std::string, std::vector< char > > RenderSurface::cachedShaders { };

RenderSurface::RenderSurface( const std::shared_ptr< RenderContext > &context,
                              std::vector< Shader > shaders )
        : context( context ), shaders( std::move( shaders ) ) {
    pipelineCreateInfo.pDepthStencilState = nullptr;

    msaaSampleCount = RenderUtilities::maxDeviceMSAASampleCount( context->physicalDevice );

    createPipeline( false );

    context->subscribeToEvent( EventType::SwapChainInvalidated, [ & ](
            RenderContext *context, EventType eventType ) -> void {
        context->logicalDevice.waitIdle( );

        dispose( );
        createPipeline( true );
    } );
}

void RenderSurface::createPipeline( bool isReset ) {

#define IFISNOTRESET( F ) if ( !isReset ) { F; }

    createSurface( );

    IFISNOTRESET( configureVertexInput( ) )

    configureViewport( );

    IFISNOTRESET( configureRasterization( ) )

    IFISNOTRESET( configureMultisampling( ) )

    IFISNOTRESET( configureColorBlend( ) )

    configureDynamicState( );

    IFISNOTRESET( createDescriptorPool( ) );

    if ( context->descriptorManager == nullptr ) {
        context->descriptorManager = std::make_shared< DescriptorManager >( context, shaderLayout );
    }

    if ( renderer == nullptr ) {
        renderer = std::make_shared< Renderer >( context, shaderLayout );
    }

    IFISNOTRESET( createPipelineLayout( ) )

    createSamplingResources( );

    createDepthAttachmentImages( );

    IFISNOTRESET( createRenderPass( ) )

    context->pipeline = context->logicalDevice.createGraphicsPipeline( nullptr, pipelineCreateInfo ).value;

    createFrameBuffers( );
}

void RenderSurface::createSurface( ) {
    VkSurfaceCapabilitiesKHR capabilities;

    capabilities = context->physicalDevice.getSurfaceCapabilitiesKHR( context->surface );
    auto formats = context->physicalDevice.getSurfaceFormatsKHR( context->surface );
    auto presentModes = context->physicalDevice.getSurfacePresentModesKHR( context->surface );

    std::function< bool( vk::SurfaceFormatKHR ) > formatCondition = [ ]( vk::SurfaceFormatKHR format ) -> bool {
        return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    };

    std::function< bool( vk::PresentModeKHR ) > presentModeCondition = [ ]( vk::PresentModeKHR presentMode ) -> bool {
        return presentMode == vk::PresentModeKHR::eMailbox;
    };

    vk::SurfaceFormatKHR selectedSurfaceFormat = Utilities::matchAndGetOrDefault( formats[ 0 ], formats,
                                                                                  formatCondition );

    vk::PresentModeKHR selectedPresentMode = Utilities::matchAndGetOrDefault( vk::PresentModeKHR::eFifo, presentModes,
                                                                              presentModeCondition );

    context->imageFormat = selectedSurfaceFormat.format;

    createSwapChain( capabilities, selectedSurfaceFormat, selectedPresentMode );
}

void RenderSurface::createSwapChain( vk::SurfaceCapabilitiesKHR surfaceCapabilities,
                                     vk::SurfaceFormatKHR surfaceFormat,
                                     vk::PresentModeKHR presentMode ) {
    chooseExtent2D( surfaceCapabilities );

    vk::SwapchainCreateInfoKHR createInfo { };

    uint32_t imageCount = std::min( surfaceCapabilities.maxImageCount, surfaceCapabilities.minImageCount + 1 );

    createInfo.surface = context->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = context->surfaceExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    const uint32_t qfIndexes[2] = { context->queueFamilies[ QueueType::Graphics ].index,
                                    context->queueFamilies[ QueueType::Presentation ].index };

    if ( qfIndexes[ 0 ] != qfIndexes[ 1 ] ) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = qfIndexes;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    context->swapChain = context->logicalDevice.createSwapchainKHR( createInfo );

    createSwapChainImages( surfaceFormat.format );
}

void RenderSurface::createSwapChainImages( vk::Format format ) {
    context->swapChainImages = context->logicalDevice.getSwapchainImagesKHR( context->swapChain );

    context->imageViews.resize( context->swapChainImages.size() );

    int index = 0;
    for ( auto image: context->swapChainImages ) {
        createImageView( context->imageViews[ index++ ], image, format, vk::ImageAspectFlagBits::eColor );
    }
}

void RenderSurface::chooseExtent2D( const vk::SurfaceCapabilitiesKHR &capabilities ) {
    if ( capabilities.currentExtent.width != UINT32_MAX ) {
        context->surfaceExtent.width = capabilities.currentExtent.width;
        context->surfaceExtent.height = capabilities.currentExtent.height;
        return;
    }

    int width;
    int height;

    glfwGetFramebufferSize( context->window, &width, &height );

    auto w = static_cast<uint32_t>( width );
    auto h = static_cast<uint32_t>( height );

    context->surfaceExtent.width = std::clamp( w, capabilities.minImageExtent.width,
                                               capabilities.maxImageExtent.width );
    context->surfaceExtent.height = std::clamp( h, capabilities.minImageExtent.height,
                                                capabilities.maxImageExtent.height );

}

void RenderSurface::configureVertexInput( ) {
    for ( const Shader &shader: shaders ) {
        vk::PipelineShaderStageCreateInfo createInfo { };

        vk::ShaderStageFlagBits shaderStage { };

        switch ( shader.type ) {
            case ShaderType::Vertex:
                shaderStage = vk::ShaderStageFlagBits::eVertex;
                break;
            case ShaderType::Fragment:
                shaderStage = vk::ShaderStageFlagBits::eFragment;
                break;
        }

        vk::ShaderModule shaderModule = this->createShaderModule( shader.filename );
        createInfo.stage = shaderStage;
        createInfo.module = shaderModule;
        createInfo.pName = "main";
        createInfo.pNext = nullptr;

        pipelineStageCreateInfos.emplace_back( createInfo );
        shaderModules.emplace_back( shaderModule );
    }

    const auto &attributeDescription = shaderLayout->getVertexAttributeDescriptions( );
    const auto &bindingDescription = shaderLayout->getInputBindingDescription( );

    inputStateCreateInfo.vertexBindingDescriptionCount = 1;
    inputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    inputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescription.size( );
    inputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription.data( );

    inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    pipelineCreateInfo.stageCount = static_cast< uint32_t >( pipelineStageCreateInfos.size( ) );
    pipelineCreateInfo.pStages = pipelineStageCreateInfos.data( );
    pipelineCreateInfo.pVertexInputState = &inputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
}

void RenderSurface::configureMultisampling( ) {
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = msaaSampleCount;
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
    multisampleStateCreateInfo.minSampleShading = .2f;

    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
}

void RenderSurface::configureViewport( ) {
    context->viewport.x = 0.0f;
    context->viewport.y = 0.0f;
    context->viewport.width = context->surfaceExtent.width;
    context->viewport.height = context->surfaceExtent.height;
    context->viewport.minDepth = 0.0f;
    context->viewport.maxDepth = 1.0f;

    viewScissor.offset = vk::Offset2D { 0, 0 };
    viewScissor.extent = context->surfaceExtent;

    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &context->viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &viewScissor;

    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
}

void RenderSurface::configureRasterization( ) {
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterizationStateCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
}

void RenderSurface::configureColorBlend( ) {
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = false;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    // This overwrites the above
    colorBlending.logicOpEnable = false;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[ 0 ] = 0.0f;
    colorBlending.blendConstants[ 1 ] = 0.0f;
    colorBlending.blendConstants[ 2 ] = 0.0f;
    colorBlending.blendConstants[ 3 ] = 0.0f;

    pipelineCreateInfo.pColorBlendState = &colorBlending;
}

void
RenderSurface::configureDynamicState( ) {
#ifdef CODE_COMMENTED
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
#endif
}

void
RenderSurface::createPipelineLayout( ) {
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &context->descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    context->pipelineLayout = context->logicalDevice.createPipelineLayout( pipelineLayoutCreateInfo );
    pipelineCreateInfo.layout = context->pipelineLayout;
}

void RenderSurface::createRenderPass( ) {
    // Color Attachment
    vk::AttachmentDescription colorAttachmentDescription { };
    colorAttachmentDescription.format = context->imageFormat;
    colorAttachmentDescription.samples = msaaSampleCount;
    colorAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference colorAttachmentReference { };
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    // --

    // Depth attachment
    vk::AttachmentDescription depthAttachmentDescription { };
    depthAttachmentDescription.format = findSupportedDepthFormat( );
    depthAttachmentDescription.samples = msaaSampleCount;
    depthAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachmentDescription.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference depthAttachmentReference { };
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    // --

    // Color Image Resolver for MSAA
    vk::AttachmentDescription colorAttachmentResolve { };
    colorAttachmentResolve.format = context->imageFormat;
    colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentResolveReference { };
    colorAttachmentResolveReference.attachment = 2;
    colorAttachmentResolveReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    // --

    vk::SubpassDescription subPass { };
    subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentReference;
    subPass.pDepthStencilAttachment = &depthAttachmentReference;
    subPass.pResolveAttachments = &colorAttachmentResolveReference;

    std::array< vk::AttachmentDescription, 3 > attachments { colorAttachmentDescription, depthAttachmentDescription,
                                                             colorAttachmentResolve };

    vk::SubpassDependency dependency { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = { }; // TODO Recheck
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassCreateInfo { };
    renderPassCreateInfo.attachmentCount = attachments.size( );
    renderPassCreateInfo.pAttachments = attachments.data( );
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subPass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    context->renderPass = context->logicalDevice.createRenderPass( renderPassCreateInfo );

    pipelineCreateInfo.renderPass = context->renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;
}

std::vector< char > RenderSurface::readFile( const std::string &filename ) {
    if ( cachedShaders.find( filename ) != cachedShaders.end( ) ) {
        return cachedShaders[ filename ];
    }

    std::ifstream file( filename, std::ios::ate | std::ios::binary );

    if ( !file.is_open( ) ) {
        throw std::runtime_error( "failed to open file!" );
    }

    size_t fileSize = static_cast<size_t>( file.tellg( ) );
    std::vector< char > contents( fileSize );

    file.seekg( 0 );
    file.read( contents.data( ), fileSize );

    file.close( );

    cachedShaders[ filename ] = std::move( contents );

    return cachedShaders[ filename ];
}

vk::ShaderModule RenderSurface::createShaderModule( const std::string &filename ) {
    std::vector< char > data = readFile( filename );

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo { };
    shaderModuleCreateInfo.codeSize = data.size( );
    shaderModuleCreateInfo.pCode = reinterpret_cast< const uint32_t * >( data.data( ) );

    vk::ShaderModule shaderModule;

    return context->logicalDevice.createShaderModule( shaderModuleCreateInfo );
}

void RenderSurface::createFrameBuffers( ) {
    auto imageViews = context->imageViews;
    context->frameBuffers.resize( imageViews.size( ) );

    int index = 0;
    for ( vk::ImageView &imageView: imageViews ) {
        std::array< vk::ImageView, 3 > attachments = {
                samplingImageView,
                context->depthView,
                imageView
        };

        vk::FramebufferCreateInfo framebufferCreateInfo { };
        framebufferCreateInfo.renderPass = context->renderPass;
        framebufferCreateInfo.attachmentCount = attachments.size( );
        framebufferCreateInfo.pAttachments = attachments.data( );
        framebufferCreateInfo.width = context->surfaceExtent.width;
        framebufferCreateInfo.height = context->surfaceExtent.height;
        framebufferCreateInfo.layers = 1;

        context->frameBuffers[ index ] = context->logicalDevice.createFramebuffer( framebufferCreateInfo );
        ++index;
    }
}

void RenderSurface::createImageView( vk::ImageView &imageView, const vk::Image &image,
                                     const vk::Format &format, const vk::ImageAspectFlags &aspectFlags
) {
    vk::ImageViewCreateInfo imageViewCreateInfo { };
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    imageView = context->logicalDevice.createImageView( imageViewCreateInfo );
}

void RenderSurface::createSamplingResources( ) {
    vk::ImageCreateInfo imageCreateInfo { };
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = context->surfaceExtent.width;
    imageCreateInfo.extent.height = context->surfaceExtent.width;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = context->imageFormat;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = msaaSampleCount;

    samplingImage = context->logicalDevice.createImage( imageCreateInfo );

    vk::MemoryRequirements requirements;

    RenderUtilities::allocateImageMemory( context, samplingImage, samplingMemory, requirements,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal );

    context->logicalDevice.bindImageMemory( samplingImage, samplingMemory, 0 );

    createImageView( samplingImageView, samplingImage, context->imageFormat, vk::ImageAspectFlagBits::eColor );
}

void RenderSurface::createDepthAttachmentImages( ) {
    const vk::Format &format = findSupportedDepthFormat( );

    vk::ImageCreateInfo imageCreateInfo { };

    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = context->surfaceExtent.width;
    imageCreateInfo.extent.height = context->surfaceExtent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = msaaSampleCount;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;;

    context->depthImage = context->logicalDevice.createImage( imageCreateInfo );

    vk::MemoryRequirements memRequirements =
            context->logicalDevice.getImageMemoryRequirements( context->depthImage );

    RenderUtilities::allocateImageMemory( context, context->depthImage, context->depthMemory, memRequirements,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal );

    context->logicalDevice.bindImageMemory( context->depthImage, context->depthMemory, 0 );


    createImageView( context->depthView, context->depthImage, format, vk::ImageAspectFlagBits::eDepth );

    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eLess;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = vk::StencilOpState { };
    depthStencilStateCreateInfo.back = vk::StencilOpState { };

    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
}

vk::Format RenderSurface::findSupportedDepthFormat( ) {
    vk::Format desiredFormats[] = { vk::Format::eD24UnormS8Uint, vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat };

    for ( auto format: desiredFormats ) {
        vk::FormatProperties properties = context->physicalDevice.getFormatProperties( format );

        if ( ( properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment )
             == vk::FormatFeatureFlagBits::eDepthStencilAttachment ) {
            return format;
        }
    }

    return vk::Format::eD32Sfloat;
}

RenderSurface::~RenderSurface( ) {
    for ( auto& module: shaderModules ) {
        context->logicalDevice.destroyShaderModule( module );
    }

    dispose( );
    context->logicalDevice.destroyDescriptorPool( context->descriptorPool );
    context->logicalDevice.destroyDescriptorSetLayout( context->descriptorSetLayout );
    context->logicalDevice.destroyPipelineLayout( context->pipelineLayout );
    context->logicalDevice.destroyRenderPass( context->renderPass );
}

std::shared_ptr< Renderer > &RenderSurface::getSurfaceRenderer( ) {
    return renderer;
}

void RenderSurface::dispose( ) {
    if ( renderer != nullptr ) {
        renderer->freeBuffers( );
    }

    context->logicalDevice.destroyImageView( samplingImageView );
    context->logicalDevice.destroyImage( samplingImage );
    context->logicalDevice.freeMemory( samplingMemory );

    context->logicalDevice.destroyImageView( context->depthView );
    context->logicalDevice.destroyImage( context->depthImage );
    context->logicalDevice.freeMemory( context->depthMemory );

    for ( auto& buffer: context->frameBuffers ) {
        context->logicalDevice.destroyFramebuffer( buffer );
    }

    context->logicalDevice.destroyPipeline( context->pipeline );

    for ( auto& imageView: context->imageViews ) {
        context->logicalDevice.destroyImageView( imageView );
    }

    context->logicalDevice.destroySwapchainKHR( context->swapChain );
}

void RenderSurface::createDescriptorPool( ) {
    vk::DescriptorPoolSize poolSize { };

    auto swapChainImageCount = static_cast< uint32_t >( context->swapChainImages.size( ) );

    poolSize.type = vk::DescriptorType::eUniformBuffer;
    poolSize.descriptorCount = swapChainImageCount;

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo { };
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &poolSize;
    descriptorPoolCreateInfo.maxSets = swapChainImageCount;

    context->descriptorPool = context->logicalDevice.createDescriptorPool( descriptorPoolCreateInfo );
}

END_NAMESPACES