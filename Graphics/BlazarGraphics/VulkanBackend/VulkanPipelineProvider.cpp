#include <BlazarCore/Utilities.h>
#include "VulkanPipelineProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< IPipeline > VulkanPipelineProvider::createPipeline( const PipelineRequest &request )
{
    ASSERT_M( request.parentPass != nullptr, "You must provide a parent render pass" );

    auto pipeline = std::make_shared< VulkanPipeline >( );
    pipeline->context = context;

    std::vector< GLSLShaderInfo > glslShaders{ };

    // todo make sure maybe have a dynamic path once multiple render apis are supported
    if ( request.enabledPipelineStages.vertex )
    {
        auto& vertexShader = glslShaders.emplace_back( );
        vertexShader.type = vk::ShaderStageFlagBits::eVertex;
        vertexShader.path = request.vertexShaderPath;
    }

    if ( request.enabledPipelineStages.fragment )
    {
        auto& vertexShader = glslShaders.emplace_back( );
        vertexShader.type = vk::ShaderStageFlagBits::eFragment;
        vertexShader.path = request.fragmentShaderPath;
    }

    // TODO cache depending on the pipeline, or maybe cache somewhere else
    createPipeline( request, pipeline, glslShaders );

    pipelineInstances.push_back( std::move( pipeline ) );
    return pipelineInstances[ pipelineInstances.size( ) - 1 ];
}

void VulkanPipelineProvider::createPipeline( const PipelineRequest &request, const std::shared_ptr< VulkanPipeline > &instance, const std::vector< GLSLShaderInfo > &shaderInfos )
{
    PipelineCreateInfos createInfo { };
    createInfo.request = request;

    auto glslShaderSet = std::make_shared< GLSLShaderSet >( shaderInfos );

    instance->descriptorManager = std::make_shared< DescriptorManager >( context, glslShaderSet );

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

    instance->pipeline = context->logicalDevice.createGraphicsPipeline( nullptr, createInfo.pipelineCreateInfo ).value;
}

void VulkanPipelineProvider::configureVertexInput( PipelineCreateInfos &createInfo )
{
    for ( const GLSLShaderInfo &shader: createInfo.shaders )
    {
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

void VulkanPipelineProvider::configureMultisampling( PipelineCreateInfos &createInfo )
{
    createInfo.multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

    if ( createInfo.request.parentPass->getProperty( "UseMSAA" ) == "true" )
    {
        createInfo.multisampleStateCreateInfo.rasterizationSamples = VulkanUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
    }
    else
    {
        createInfo.multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    }

    createInfo.multisampleStateCreateInfo.minSampleShading = 1.0f;
    createInfo.multisampleStateCreateInfo.pSampleMask = nullptr;
    createInfo.multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    createInfo.multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    createInfo.multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
    createInfo.multisampleStateCreateInfo.minSampleShading = .2f;

    createInfo.pipelineCreateInfo.pMultisampleState = &createInfo.multisampleStateCreateInfo;
}

void VulkanPipelineProvider::configureViewport( PipelineCreateInfos &createInfo )
{
    createInfo.viewScissor.offset = vk::Offset2D { 0, 0 };
    createInfo.viewScissor.extent = context->surfaceExtent;

    createInfo.viewportStateCreateInfo.viewportCount = 1;
    createInfo.viewportStateCreateInfo.pViewports = &context->viewport;
    createInfo.viewportStateCreateInfo.scissorCount = 1;
    createInfo.viewportStateCreateInfo.pScissors = &createInfo.viewScissor;

    createInfo.pipelineCreateInfo.pViewportState = &createInfo.viewportStateCreateInfo;
}

void VulkanPipelineProvider::configureRasterization( PipelineCreateInfos &createInfo )
{
    createInfo.rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.rasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    createInfo.rasterizationStateCreateInfo.lineWidth = 1.0f;

    switch ( createInfo.request.cullMode )
    {
        case ECS::CullMode::FrontAndBackFace:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eFrontAndBack;
            break;
        case ECS::CullMode::BackFace:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
            break;
        case ECS::CullMode::FrontFace:
            createInfo.rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eFront;
            break;
        case ECS::CullMode::None:
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

void VulkanPipelineProvider::configureColorBlend( PipelineCreateInfos &createInfo )
{
    int attachmentCount = std::stoi( createInfo.request.parentPass->getProperty( "AttachmentCount" ) );

    createInfo.colorBlendAttachments.resize( attachmentCount );

    for ( int i = 0; i < attachmentCount; ++i )
    {
        createInfo.colorBlendAttachments[ i ].colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                         vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        createInfo.colorBlendAttachments[ i ].blendEnable = false;
        createInfo.colorBlendAttachments[ i ].srcColorBlendFactor = vk::BlendFactor::eOne;
        createInfo.colorBlendAttachments[ i ].dstColorBlendFactor = vk::BlendFactor::eZero;
        createInfo.colorBlendAttachments[ i ].colorBlendOp = vk::BlendOp::eAdd;
        createInfo.colorBlendAttachments[ i ].srcAlphaBlendFactor = vk::BlendFactor::eOne;
        createInfo.colorBlendAttachments[ i ].dstAlphaBlendFactor = vk::BlendFactor::eZero;
        createInfo.colorBlendAttachments[ i ].alphaBlendOp = vk::BlendOp::eAdd;
    }

    // This overwrites the above
    createInfo.colorBlending.logicOpEnable = false;
    createInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    createInfo.colorBlending.attachmentCount = attachmentCount;
    createInfo.colorBlending.pAttachments = createInfo.colorBlendAttachments.data( );
    createInfo.colorBlending.blendConstants[ 0 ] = 0.0f;
    createInfo.colorBlending.blendConstants[ 1 ] = 0.0f;
    createInfo.colorBlending.blendConstants[ 2 ] = 0.0f;
    createInfo.colorBlending.blendConstants[ 3 ] = 0.0f;

    createInfo.pipelineCreateInfo.pColorBlendState = &createInfo.colorBlending;
}

void VulkanPipelineProvider::configureDynamicState( PipelineCreateInfos &createInfo )
{
    createInfo.dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size( );
    createInfo.dynamicStateCreateInfo.pDynamicStates = dynamicStates.data( );

    createInfo.pipelineCreateInfo.pDynamicState = &createInfo.dynamicStateCreateInfo;
}

void VulkanPipelineProvider::createPipelineLayout( PipelineCreateInfos &createInfo, const std::shared_ptr< VulkanPipeline > &instance )
{
    const std::vector< vk::DescriptorSetLayout > &layouts = instance->descriptorManager->getLayouts( );
    createInfo.pipelineLayoutCreateInfo.setLayoutCount = layouts.size( );
    createInfo.pipelineLayoutCreateInfo.pSetLayouts = layouts.data( );

    const std::vector< vk::PushConstantRange > &pushConstants = createInfo.shaderSet->getPushConstants( );

    createInfo.pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstants.size( );
    createInfo.pipelineLayoutCreateInfo.pPushConstantRanges = pushConstants.data( );

    instance->layout = context->logicalDevice.createPipelineLayout( createInfo.pipelineLayoutCreateInfo );
    createInfo.pipelineCreateInfo.layout = instance->layout;
}

void VulkanPipelineProvider::createRenderPass( PipelineCreateInfos &createInfo )
{
    createInfo.pipelineCreateInfo.renderPass = std::dynamic_pointer_cast< VulkanRenderPass >( createInfo.request.parentPass )->getPassInstance( );
    createInfo.pipelineCreateInfo.subpass = 0;
    createInfo.pipelineCreateInfo.basePipelineHandle = nullptr;
    createInfo.pipelineCreateInfo.basePipelineIndex = -1;
}

void VulkanPipelineProvider::createDepthAttachmentImages( PipelineCreateInfos &createInfo )
{
    createInfo.depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;//createInfo.options.depthTestEnable;
    createInfo.depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE; //createInfo.options.depthTestEnable;

    switch ( createInfo.request.depthCompareOp )
    {
        case CompareOp::Less:
            createInfo.depthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eLess;
            break;
        case CompareOp::LessOrEqual:
            createInfo.depthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eLessOrEqual;
            break;
        case CompareOp::Greater:
            createInfo.depthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eGreater;
            break;
        case CompareOp::GreaterOrEqual:
            createInfo.depthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eGreaterOrEqual;
            break;
    }

    createInfo.depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    createInfo.depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    createInfo.depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    createInfo.depthStencilStateCreateInfo.front = vk::StencilOpState { };
    createInfo.depthStencilStateCreateInfo.back = vk::StencilOpState { };

    createInfo.pipelineCreateInfo.pDepthStencilState = &createInfo.depthStencilStateCreateInfo;
}

vk::ShaderModule VulkanPipelineProvider::createShaderModule( const std::string &filename )
{
    std::vector< char > data = Core::Utilities::readFile( filename );

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo { };
    shaderModuleCreateInfo.codeSize = data.size( );
    shaderModuleCreateInfo.pCode = reinterpret_cast< const uint32_t * >( data.data( ) );

    return context->logicalDevice.createShaderModule( shaderModuleCreateInfo );
}

VulkanPipelineProvider::~VulkanPipelineProvider( )
{
    for ( auto &module: shaderModules )
    {
        context->logicalDevice.destroyShaderModule( module );
    }

    for ( auto& instance: pipelineInstances )
    {
        instance.reset( );
    }
}
END_NAMESPACES