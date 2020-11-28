//
// Created by Murat on 10/18/2020.
//

#include <fstream>
#include <utility>
#include "RenderSurface.h"

NAMESPACES( SomeVulkan, Graphics )

std::unordered_map< std::string, std::vector< char > > RenderSurface::cachedShaders { };

RenderSurface::RenderSurface( const std::shared_ptr< InstanceContext > &context, std::shared_ptr< Scene::Camera > camera )
        : context( context ), camera( std::move( camera ) ) {
    pipelineSelector = std::make_shared< PipelineSelector >( );
    msaaSampleCount = RenderUtilities::maxDeviceMSAASampleCount( context->physicalDevice );

    createSurface( );
    createPipelines( );
    createDepthImages( );
    createSamplingResources( );
    createFrameBuffers( );

    enginePipelineSelector = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) {
        if ( entity->hasComponent< CMesh >()) {
            if ( entity->getComponent< CMesh >()->cullMode == CullMode::None ) {
                return ENGINE_CORE_PIPELINE_NONE_CULL;
            }
        }

        return ENGINE_CORE_PIPELINE_BACK_CULL;
    };

    pipelineSelector->addSelector( PipelineSelectorPair { 1, enginePipelineSelector } );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::WindowResized, [ & ]( const Input::EventType &eventType, std::shared_ptr< Input::IEventParameters > eventParams ) {
        auto parameters = Input::GlobalEventHandler::ToWindowResizedParameters( eventParams );
        context->logicalDevice.waitIdle( );

        updateViewport( parameters->width, parameters->height );
    } );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::SwapChainInvalidated, [ & ]( const Input::EventType &eventType, std::shared_ptr< Input::IEventParameters > eventParams ) {
        context->logicalDevice.waitIdle( );

        dispose( );
        createSurface( );
        createDepthImages( );
        createSamplingResources( );
        createFrameBuffers( );
    } );

    if ( renderer == nullptr ) {
        renderer = std::make_shared< Renderer >( context, this->camera, pipelineSelector );
    }
}

void RenderSurface::createPipelines( ) {
    // create default pipelines
    pipelineInstances.resize( 2 );
    std::vector< Graphics::ShaderInfo > shaders( 2 );

    shaders[ 0 ].type = vk::ShaderStageFlagBits::eVertex;
    shaders[ 0 ].path = PATH( "/Shaders/SPIRV/Vertex/default.spv" );
    shaders[ 1 ].type = vk::ShaderStageFlagBits::eFragment;
    shaders[ 1 ].path = PATH( "/Shaders/SPIRV/Fragment/default.spv" );

    ///////////////////////////////////////////////////////////////////
    // Engine Core Pipeline with back face culling ////////////////////
    ///////////////////////////////////////////////////////////////////
    PipelineInstance &instance = pipelineInstances.emplace_back( );
    instance.name = ENGINE_CORE_PIPELINE_BACK_CULL;

    PipelineOptions options { };
    options.cullMode = CullMode::BackFace;

    createPipeline( options, instance, shaders );
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Engine Core Pipeline with back face culling ////////////////////
    ///////////////////////////////////////////////////////////////////
    PipelineInstance &instance2 = pipelineInstances.emplace_back( );
    instance2.name = ENGINE_CORE_PIPELINE_NONE_CULL;

    options = { };
    options.cullMode = CullMode::None;

    createPipeline( options, instance2, shaders );
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    pipelineSelector->createPipelineInstance( instance );
    pipelineSelector->createPipelineInstance( instance2 );
}

void RenderSurface::createPipeline( const PipelineOptions &options, PipelineInstance &instance, const std::vector< ShaderInfo > &shaderInfos ) {
    PipelineCreateInfos createInfo { };
    createInfo.options = options;

    auto glslShaderSet = std::make_shared< GLSLShaderSet >( shaderInfos );

    instance.descriptorManager = std::make_shared< DescriptorManager >( context, glslShaderSet );

    createInfo.shaders = shaderInfos;
    createInfo.shaderSet = glslShaderSet;

    createInfo.pipelineCreateInfo.pDepthStencilState = nullptr;

    configureVertexInput( createInfo );

    configureViewport( createInfo );

    configureRasterization( createInfo );

    configureMultisampling( createInfo );

    configureColorBlend( createInfo );

    configureDynamicState( createInfo );

    createPipelineLayout( createInfo, instance );

    createDepthAttachmentImages( createInfo );

    createRenderPass( createInfo );

    instance.pipeline = context->logicalDevice.createGraphicsPipeline( nullptr, createInfo.pipelineCreateInfo ).value;
}

void RenderSurface::createSurface( ) {
    vk::SurfaceCapabilitiesKHR capabilities;

    capabilities = context->physicalDevice.getSurfaceCapabilitiesKHR( context->surface );

    createSwapChain( capabilities );
}

void RenderSurface::createSwapChain( const vk::SurfaceCapabilitiesKHR &surfaceCapabilities ) {
    chooseExtent2D( surfaceCapabilities );
    updateViewport( surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height );

    vk::SwapchainCreateInfoKHR createInfo { };

    uint32_t imageCount = std::min( surfaceCapabilities.maxImageCount, surfaceCapabilities.minImageCount + 1 );

    createInfo.surface = context->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = context->imageFormat;
    createInfo.imageColorSpace = context->colorSpace;
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
    createInfo.presentMode = context->presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = context->swapChain;

    context->swapChain = context->logicalDevice.createSwapchainKHR( createInfo );

    createSwapChainImages( context->imageFormat );
}

void RenderSurface::createSwapChainImages( vk::Format format ) {
    context->swapChainImages = context->logicalDevice.getSwapchainImagesKHR( context->swapChain );

    context->imageViews.resize( context->swapChainImages.size( ) );

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

    context->surfaceExtent.width = std::clamp( w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
    context->surfaceExtent.height = std::clamp( h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
}

void RenderSurface::updateViewport( const uint32_t &width, const uint32_t &height ) {
    context->viewport.x = 0.0f;
    context->viewport.y = 0.0f;
    context->viewport.width = width;
    context->viewport.height = height;
    context->viewport.minDepth = 0.0f;
    context->viewport.maxDepth = 1.0f;

    context->viewScissor.offset = vk::Offset2D( 0, 0 );
    context->viewScissor.extent = vk::Extent2D( width, height );
}

void RenderSurface::configureVertexInput( PipelineCreateInfos &createInfo ) {
    for ( const ShaderInfo &shader: createInfo.shaders ) {
        vk::PipelineShaderStageCreateInfo shaderStageCreateInfo { };

        vk::ShaderModule shaderModule = this->createShaderModule( shader.path );
        shaderStageCreateInfo.stage = shader.type;
        shaderStageCreateInfo.module = shaderModule;
        shaderStageCreateInfo.pName = "main";
        shaderStageCreateInfo.pNext = nullptr;

        createInfo.pipelineStageCreateInfos.emplace_back( shaderStageCreateInfo );
        shaderModules.emplace_back( shaderModule );
    }

    const auto &attributeDescription = createInfo.shaderSet->getVertexAttributeDescriptions( );
    const auto &bindingDescriptions = createInfo.shaderSet->getInputBindingDescriptions( );

    createInfo.inputStateCreateInfo.vertexBindingDescriptionCount = bindingDescriptions.size( );
    createInfo.inputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data( );
    createInfo.inputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescription.size( );
    createInfo.inputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription.data( );

    createInfo.inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    createInfo.inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    createInfo.pipelineCreateInfo.stageCount = static_cast< uint32_t >( createInfo.pipelineStageCreateInfos.size( ) );
    createInfo.pipelineCreateInfo.pStages = createInfo.pipelineStageCreateInfos.data( );
    createInfo.pipelineCreateInfo.pVertexInputState = &createInfo.inputStateCreateInfo;
    createInfo.pipelineCreateInfo.pInputAssemblyState = &createInfo.inputAssemblyCreateInfo;
}

void RenderSurface::configureMultisampling( PipelineCreateInfos &createInfo ) {
    createInfo.multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    createInfo.multisampleStateCreateInfo.rasterizationSamples = msaaSampleCount;
    createInfo.multisampleStateCreateInfo.minSampleShading = 1.0f;
    createInfo.multisampleStateCreateInfo.pSampleMask = nullptr;
    createInfo.multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    createInfo.multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    createInfo.multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
    createInfo.multisampleStateCreateInfo.minSampleShading = .2f;

    createInfo.pipelineCreateInfo.pMultisampleState = &createInfo.multisampleStateCreateInfo;
}

void RenderSurface::configureViewport( PipelineCreateInfos &createInfo ) {
    createInfo.viewScissor.offset = vk::Offset2D { 0, 0 };
    createInfo.viewScissor.extent = context->surfaceExtent;

    createInfo.viewportStateCreateInfo.viewportCount = 1;
    createInfo.viewportStateCreateInfo.pViewports = &context->viewport;
    createInfo.viewportStateCreateInfo.scissorCount = 1;
    createInfo.viewportStateCreateInfo.pScissors = &createInfo.viewScissor;

    createInfo.pipelineCreateInfo.pViewportState = &createInfo.viewportStateCreateInfo;
}

void RenderSurface::configureRasterization( PipelineCreateInfos &createInfo ) {
    createInfo.rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizationStateCreateInfo.lineWidth = 1.0f;

    switch ( createInfo.options.cullMode ) {
        case CullMode::FrontAndBackFace:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eFrontAndBack;
            break;
        case CullMode::BackFace:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
            break;
        case CullMode::FrontFace:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eFront;
            break;
        case CullMode::None:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eNone;
            break;
    }

    createInfo.rasterizationStateCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
    createInfo.rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    createInfo.rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    createInfo.rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    createInfo.rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

    createInfo.pipelineCreateInfo.pRasterizationState = &createInfo.rasterizationStateCreateInfo;
}

void RenderSurface::configureColorBlend( PipelineCreateInfos &createInfo ) {
    createInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                     vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    createInfo.colorBlendAttachment.blendEnable = false;
    createInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    createInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    createInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    createInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    createInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    createInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    // This overwrites the above
    createInfo.colorBlending.logicOpEnable = false;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = 1;
    createInfo.colorBlending.pAttachments = &createInfo.colorBlendAttachment;
    createInfo.colorBlending.blendConstants[ 0 ] = 0.0f;
    createInfo.colorBlending.blendConstants[ 1 ] = 0.0f;
    createInfo.colorBlending.blendConstants[ 2 ] = 0.0f;
    createInfo.colorBlending.blendConstants[ 3 ] = 0.0f;

    createInfo.pipelineCreateInfo.pColorBlendState = &createInfo.colorBlending;
}

void RenderSurface::configureDynamicState( PipelineCreateInfos &createInfo ) {
    createInfo.dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size( );
    createInfo.dynamicStateCreateInfo.pDynamicStates = dynamicStates.data( );

    createInfo.pipelineCreateInfo.pDynamicState = &createInfo.dynamicStateCreateInfo;
}

void RenderSurface::createPipelineLayout( PipelineCreateInfos &createInfo, PipelineInstance &instance ) {
    const std::vector< vk::DescriptorSetLayout > &layouts = instance.descriptorManager->getLayouts( );
    createInfo.pipelineLayoutCreateInfo.setLayoutCount = layouts.size( );
    createInfo.pipelineLayoutCreateInfo.pSetLayouts = layouts.data( );

    createInfo.pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    createInfo.pushConstantRange.offset = 0;
    createInfo.pushConstantRange.size = 4 * 4 * sizeof( float );

    createInfo.pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    createInfo.pipelineLayoutCreateInfo.pPushConstantRanges = &createInfo.pushConstantRange;

    instance.layout = context->logicalDevice.createPipelineLayout( createInfo.pipelineLayoutCreateInfo );
    createInfo.pipelineCreateInfo.layout = instance.layout;
}

void RenderSurface::createRenderPass( PipelineCreateInfos &createInfo ) {
    createInfo.pipelineCreateInfo.renderPass = context->renderPass;
    createInfo.pipelineCreateInfo.subpass = 0;
    createInfo.pipelineCreateInfo.basePipelineHandle = nullptr;
    createInfo.pipelineCreateInfo.basePipelineIndex = -1;
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

void RenderSurface::createDepthAttachmentImages( PipelineCreateInfos &createInfo ) {
    createInfo.depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    createInfo.depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    createInfo.depthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eLess;
    createInfo.depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    createInfo.depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    createInfo.depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    createInfo.depthStencilStateCreateInfo.front = vk::StencilOpState { };
    createInfo.depthStencilStateCreateInfo.back = vk::StencilOpState { };

    createInfo.pipelineCreateInfo.pDepthStencilState = &createInfo.depthStencilStateCreateInfo;
}

void RenderSurface::createDepthImages( ) {
    const vk::Format &format = RenderUtilities::findSupportedDepthFormat( context->physicalDevice );

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
}

RenderSurface::~RenderSurface( ) {
    for ( auto &module: shaderModules ) {
        context->logicalDevice.destroyShaderModule( module );
    }

    dispose( );

    for ( PipelineInstance &instance: pipelineInstances ) {
        context->logicalDevice.destroyPipeline( instance.pipeline );
        context->logicalDevice.destroyPipelineLayout( instance.layout );
    }

    context->logicalDevice.destroySwapchainKHR( context->swapChain );
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

    for ( auto &buffer: context->frameBuffers ) {
        context->logicalDevice.destroyFramebuffer( buffer );
    }

    for ( auto &imageView: context->imageViews ) {
        context->logicalDevice.destroyImageView( imageView );
    }
}

END_NAMESPACES