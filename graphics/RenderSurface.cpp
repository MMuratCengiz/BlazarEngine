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
    pipelineCreateInfo = { };
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pDepthStencilState = nullptr;

    msaaSampleCount = RenderUtilities::maxDeviceMSAASampleCount( context->physicalDevice );

    createPipeline( false );

    context->subscribeToEvent( EventType::SwapChainInvalidated, [ & ](
            RenderContext *context, EventType eventType ) -> void {
        vkDeviceWaitIdle( context->logicalDevice );

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

    if ( vkCreateGraphicsPipelines( context->logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                    &context->pipeline ) != VK_SUCCESS ) {
        throw std::runtime_error( "failed to create graphics pipeline!" );
    }

    createFrameBuffers( );
}

void RenderSurface::createSurface( ) {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( context->physicalDevice, context->surface, &capabilities );
    vkGetPhysicalDeviceSurfaceFormatsKHR( context->physicalDevice, context->surface, &formatCount, nullptr );

    std::vector< VkSurfaceFormatKHR > formats( formatCount );

    vkGetPhysicalDeviceSurfaceFormatsKHR( context->physicalDevice, context->surface, &formatCount, formats.data( ) );
    vkGetPhysicalDeviceSurfacePresentModesKHR( context->physicalDevice, context->surface, &presentModeCount, nullptr );

    std::vector< VkPresentModeKHR > presentModes( presentModeCount );

    vkGetPhysicalDeviceSurfacePresentModesKHR( context->physicalDevice, context->surface, &presentModeCount,
                                               presentModes.data( ) );

    std::function< bool( VkSurfaceFormatKHR ) > formatCondition = [ ]( VkSurfaceFormatKHR format ) -> bool {
        return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    };

    std::function< bool( VkPresentModeKHR ) > presentModeCondition = [ ]( VkPresentModeKHR presentMode ) -> bool {
        return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
    };

    VkSurfaceFormatKHR selectedSurfaceFormat = Utilities::matchAndGetOrDefault( formats[ 0 ], formats,
                                                                                formatCondition );

    VkPresentModeKHR selectedPresentMode = Utilities::matchAndGetOrDefault( VK_PRESENT_MODE_FIFO_KHR, presentModes,
                                                                            presentModeCondition );

    context->imageFormat = selectedSurfaceFormat.format;

    createSwapChain( capabilities, selectedSurfaceFormat, selectedPresentMode );
}

void RenderSurface::createSwapChain( VkSurfaceCapabilitiesKHR surfaceCapabilities,
                                     VkSurfaceFormatKHR surfaceFormat,
                                     VkPresentModeKHR presentMode ) {
    chooseExtent2D( surfaceCapabilities );

    VkSwapchainCreateInfoKHR createInfo { };

    uint32_t imageCount = std::min( surfaceCapabilities.maxImageCount, surfaceCapabilities.minImageCount + 1 );

    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = context->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = context->surfaceExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t qfIndexes[2] = { context->queueFamilies[ QueueType::Graphics ].index,
                                    context->queueFamilies[ QueueType::Presentation ].index };

    if ( qfIndexes[ 0 ] != qfIndexes[ 1 ] ) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = qfIndexes;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if ( vkCreateSwapchainKHR( context->logicalDevice, &createInfo, nullptr, &context->swapChain ) ) {
        TRACE( COMPONENT_VKPRESENTATION, VERBOSITY_CRITICAL, "Failed to create swap chain." )
    }

    createSwapChainImages( surfaceFormat.format );
}

void RenderSurface::createSwapChainImages( VkFormat format ) {
    uint32_t imageCount;

    vkGetSwapchainImagesKHR( context->logicalDevice, context->swapChain, &imageCount, nullptr );

    context->swapChainImages.resize( imageCount );
    context->imageViews.resize( imageCount );

    vkGetSwapchainImagesKHR( context->logicalDevice, context->swapChain, &imageCount,
                             context->swapChainImages.data( ) );

    int index = 0;
    for ( auto image: context->swapChainImages ) {
        createImageView( context->imageViews[ index++ ], image, format, VK_IMAGE_ASPECT_COLOR_BIT );
    }
}

void RenderSurface::chooseExtent2D( const VkSurfaceCapabilitiesKHR &capabilities ) {
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
        VkPipelineShaderStageCreateInfo createInfo { };

        VkShaderStageFlagBits shaderStage { };

        switch ( shader.type ) {
            case ShaderType::Vertex:
                shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case ShaderType::Fragment:
                shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
        }

        VkShaderModule shaderModule = this->createShaderModule( shader.filename );
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.stage = shaderStage;
        createInfo.module = shaderModule;
        createInfo.pName = "main";
        createInfo.pNext = nullptr;

        pipelineStageCreateInfos.emplace_back( createInfo );
        shaderModules.emplace_back( shaderModule );
    }

    const auto &attributeDescription = shaderLayout->getVertexAttributeDescriptions( );
    const auto &bindingDescription = shaderLayout->getInputBindingDescription( );

    inputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputStateCreateInfo.vertexBindingDescriptionCount = 1;
    inputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    inputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescription.size( );
    inputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription.data( );

    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    pipelineCreateInfo.stageCount = static_cast< uint32_t >( pipelineStageCreateInfos.size( ) );
    pipelineCreateInfo.pStages = pipelineStageCreateInfos.data( );
    pipelineCreateInfo.pVertexInputState = &inputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
}

void RenderSurface::configureMultisampling( ) {
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
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

    viewScissor.offset = { 0, 0 };
    viewScissor.extent = context->surfaceExtent;

    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &context->viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &viewScissor;

    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
}

void RenderSurface::configureRasterization( ) {
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
}

void RenderSurface::configureColorBlend( ) {
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // This overwrites the above
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
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
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
#endif
}

void
RenderSurface::createPipelineLayout( ) {
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &context->descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    if ( vkCreatePipelineLayout( context->logicalDevice, &pipelineLayoutCreateInfo, nullptr,
                                 &context->pipelineLayout ) !=
         VK_SUCCESS ) {
        throw std::runtime_error( "failed to create pipeline layout!" );
    }

    pipelineCreateInfo.layout = context->pipelineLayout;
}

void RenderSurface::createRenderPass( ) {
    // Color Attachment
    VkAttachmentDescription colorAttachmentDescription { };

    colorAttachmentDescription.format = context->imageFormat;
    colorAttachmentDescription.samples = msaaSampleCount;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentReference { };

    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // --
    // Depth attachment
    VkAttachmentDescription depthAttachmentDescription { };

    depthAttachmentDescription.format = findSupportedDepthFormat( );
    depthAttachmentDescription.samples = msaaSampleCount;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference { };

    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    // --

    // Color Image Resolver for MSAA
    VkAttachmentDescription colorAttachmentResolve { };
    colorAttachmentResolve.format = context->imageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentResolveReference { };

    colorAttachmentResolveReference.attachment = 2;
    colorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // --


    VkSubpassDescription subpass { };

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pDepthStencilAttachment = &depthAttachmentReference;
    subpass.pResolveAttachments = &colorAttachmentResolveReference;

    std::array< VkAttachmentDescription, 3 > attachments { colorAttachmentDescription, depthAttachmentDescription,
                                                           colorAttachmentResolve };

    VkSubpassDependency dependency { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo { };

    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size( );
    renderPassCreateInfo.pAttachments = attachments.data( );
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    if ( vkCreateRenderPass( context->logicalDevice, &renderPassCreateInfo, nullptr, &context->renderPass ) !=
         VK_SUCCESS ) {
        throw std::runtime_error( "failed to create render pass!" );
    }

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

VkShaderModule RenderSurface::createShaderModule( const std::string &filename ) {
    std::vector< char > data = readFile( filename );

    VkShaderModuleCreateInfo shaderModuleCreateInfo { };

    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = data.size( );
    shaderModuleCreateInfo.pCode = reinterpret_cast< const uint32_t * >( data.data( ) );

    VkShaderModule shaderModule;

    if ( vkCreateShaderModule( context->logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule ) !=
         VK_SUCCESS ) {
        throw std::runtime_error( "failed to create shader module!" );
    }

    return shaderModule;
}

void RenderSurface::createFrameBuffers( ) {
    auto imageViews = context->imageViews;
    context->frameBuffers.resize( imageViews.size( ) );

    int index = 0;
    for ( VkImageView &imageView: imageViews ) {
        std::array< VkImageView, 3 > attachments = {
                samplingImageView,
                context->depthView,
                imageView
        };

        VkFramebufferCreateInfo framebufferCreateInfo { };
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = context->renderPass;
        framebufferCreateInfo.attachmentCount = attachments.size( );
        framebufferCreateInfo.pAttachments = attachments.data( );
        framebufferCreateInfo.width = context->surfaceExtent.width;
        framebufferCreateInfo.height = context->surfaceExtent.height;
        framebufferCreateInfo.layers = 1;

        if ( vkCreateFramebuffer( context->logicalDevice, &framebufferCreateInfo, nullptr,
                                  &context->frameBuffers[ index++ ] ) !=
             VK_SUCCESS ) {
            throw std::runtime_error( "failed to create framebuffer!" );
        }
    }
}

void RenderSurface::createImageView( VkImageView &imageView, const VkImage &image,
                                     const VkFormat &format, const VkImageAspectFlags &aspectFlags
) {
    VkImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if ( vkCreateImageView( context->logicalDevice, &imageViewCreateInfo, nullptr, &imageView ) != VK_SUCCESS ) {
        TRACE( COMPONENT_VKPRESENTATION, VERBOSITY_CRITICAL, "Couldn't create image view!" )
    }
}

void RenderSurface::createSamplingResources( ) {
    VkImageCreateInfo imageCreateInfo { };

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = context->surfaceExtent.width;
    imageCreateInfo.extent.height = context->surfaceExtent.width;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = context->imageFormat;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = msaaSampleCount;

    if ( vkCreateImage( context->logicalDevice, &imageCreateInfo, nullptr, &samplingImage ) !=
         VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::Renderer, "Failed to create image!" );
    }

    VkMemoryRequirements requirements;

    RenderUtilities::allocateImageMemory( context, samplingImage, samplingMemory, requirements,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    vkBindImageMemory( context->logicalDevice, samplingImage, samplingMemory, 0 );

    createImageView( samplingImageView, samplingImage, context->imageFormat, VK_IMAGE_ASPECT_COLOR_BIT );

}

void RenderSurface::createDepthAttachmentImages( ) {
    const VkFormat &format = findSupportedDepthFormat( );

    VkImageCreateInfo imageCreateInfo { };

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = context->surfaceExtent.width;
    imageCreateInfo.extent.height = context->surfaceExtent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = msaaSampleCount;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if ( vkCreateImage( context->logicalDevice, &imageCreateInfo, nullptr, &context->depthImage ) != VK_SUCCESS ) {
        TRACE( COMPONENT_VKPRESENTATION, VERBOSITY_CRITICAL, "Couldn't create image!" )
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( context->logicalDevice, context->depthImage, &memRequirements );

    RenderUtilities::allocateImageMemory( context, context->depthImage, context->depthMemory, memRequirements,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    vkBindImageMemory( context->logicalDevice, context->depthImage, context->depthMemory, 0 );

    createImageView( context->depthView, context->depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT );


    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = { };
    depthStencilStateCreateInfo.back = { };

    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
}

VkFormat RenderSurface::findSupportedDepthFormat( ) {
    VkFormat desiredFormats[] = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT };

    for ( auto format: desiredFormats ) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties( context->physicalDevice, format, &properties );

        if ( ( properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
             == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) {
            return format;
        }
    }

    return VK_FORMAT_D32_SFLOAT;
}

RenderSurface::~RenderSurface( ) {
    for ( VkShaderModule module: shaderModules ) {
        vkDestroyShaderModule( context->logicalDevice, module, nullptr );
    }

    dispose( );

    vkDestroyDescriptorPool( context->logicalDevice, context->descriptorPool, nullptr );
    vkDestroyDescriptorSetLayout( context->logicalDevice, context->descriptorSetLayout, nullptr );
    vkDestroyPipelineLayout( context->logicalDevice, context->pipelineLayout, nullptr );
    vkDestroyRenderPass( context->logicalDevice, context->renderPass, nullptr );
}

std::shared_ptr< Renderer > &RenderSurface::getSurfaceRenderer( ) {
    return renderer;
}

void RenderSurface::dispose( ) {
    if ( renderer != nullptr ) {
        renderer->freeBuffers( );
    }

    vkDestroyImageView( context->logicalDevice, samplingImageView, nullptr );
    vkDestroyImage( context->logicalDevice, samplingImage, nullptr );
    vkFreeMemory( context->logicalDevice, samplingMemory, nullptr );

    vkDestroyImageView( context->logicalDevice, context->depthView, nullptr );
    vkDestroyImage( context->logicalDevice, context->depthImage, nullptr );
    vkFreeMemory( context->logicalDevice, context->depthMemory, nullptr );

    for ( VkFramebuffer buffer: context->frameBuffers ) {
        vkDestroyFramebuffer( context->logicalDevice, buffer, nullptr );
    }

    vkDestroyPipeline( context->logicalDevice, context->pipeline, nullptr );

    for ( auto image: context->imageViews ) {
        vkDestroyImageView( context->logicalDevice, image, nullptr );
    }

    vkDestroySwapchainKHR( context->logicalDevice, context->swapChain, nullptr );
}

void RenderSurface::createDescriptorPool( ) {
    VkDescriptorPoolSize poolSize { };

    auto swapChainImageCount = static_cast< uint32_t >( context->swapChainImages.size( ) );

    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = swapChainImageCount;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo { };
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &poolSize;
    descriptorPoolCreateInfo.maxSets = swapChainImageCount;

    if ( vkCreateDescriptorPool( context->logicalDevice, &descriptorPoolCreateInfo, nullptr,
                                 &context->descriptorPool ) !=
         VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::RenderSurface, "Failed to create descriptor pool!" );
    }
}

END_NAMESPACES