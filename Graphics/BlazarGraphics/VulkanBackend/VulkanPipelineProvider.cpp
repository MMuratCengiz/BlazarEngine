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

#include <BlazarCore/Utilities.h>
#include "VulkanPipelineProvider.h"
#include "SpirvHelper.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

IPipeline *VulkanPipelineProvider::createPipeline( const PipelineRequest &request )
{
    ASSERT_M( request.parentPass != nullptr, "You must provide a parent render pass" );

    auto pipeline = std::make_unique< VulkanPipeline >( );
    pipeline->context = context;

    std::vector< GLSLShaderInfo > glslShaders { };

    auto vertexShaderSearch = request.shaderPaths.find( ShaderType::Vertex );
    auto fragmentShaderSearch = request.shaderPaths.find( ShaderType::Fragment );
    auto tessControlShaderSearch = request.shaderPaths.find( ShaderType::TessellationControl );
    auto tessEvalShaderSearch = request.shaderPaths.find( ShaderType::TessellationEval );
    auto geometryShaderSearch = request.shaderPaths.find( ShaderType::Geometry );

    if ( vertexShaderSearch != request.shaderPaths.end( ) )
    {
        glslShaders.emplace_back( GLSLShaderInfo { vk::ShaderStageFlagBits::eVertex, vertexShaderSearch->second } );
    }

    if ( fragmentShaderSearch != request.shaderPaths.end( ) )
    {
        glslShaders.emplace_back( GLSLShaderInfo { vk::ShaderStageFlagBits::eFragment, fragmentShaderSearch->second } );
    }

    if ( tessControlShaderSearch != request.shaderPaths.end( ) )
    {
        glslShaders.emplace_back( GLSLShaderInfo { vk::ShaderStageFlagBits::eTessellationControl, tessControlShaderSearch->second } );
    }

    if ( tessEvalShaderSearch != request.shaderPaths.end( ) )
    {
        glslShaders.emplace_back( GLSLShaderInfo { vk::ShaderStageFlagBits::eTessellationEvaluation, tessEvalShaderSearch->second } );
    }

    if ( geometryShaderSearch != request.shaderPaths.end( ) )
    {
        glslShaders.emplace_back( GLSLShaderInfo { vk::ShaderStageFlagBits::eGeometry, geometryShaderSearch->second } );
    }

    // TODO cache depending on the pipeline, or maybe cache somewhere else
    createPipeline( request, pipeline.get( ), glslShaders );

    pipelineInstances.push_back( std::move( pipeline ) );
    return pipelineInstances[ pipelineInstances.size( ) - 1 ].get( );
}

void VulkanPipelineProvider::createPipeline( const PipelineRequest &request, VulkanPipeline *instance, const std::vector< GLSLShaderInfo > &shaderInfos )
{
    PipelineCreateInfos createInfo { };
    createInfo.request = request;
    createInfo.parentPass = std::dynamic_pointer_cast< VulkanRenderPass >( request.parentPass );

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
    bool hasTessellationShaders = false;

    for ( const GLSLShaderInfo &shader: createInfo.shaders )
    {
        vk::PipelineShaderStageCreateInfo shaderStageCreateInfo { };

        vk::ShaderModule shaderModule = this->createShaderModule( shader.data );
        shaderStageCreateInfo.stage = shader.type;
        shaderStageCreateInfo.module = shaderModule;
        shaderStageCreateInfo.pName = "main";
        shaderStageCreateInfo.pNext = nullptr;

        createInfo.pipelineStageCreateInfos.emplace_back( shaderStageCreateInfo );
        shaderModules.emplace_back( shaderModule );

        hasTessellationShaders = hasTessellationShaders || shader.type == vk::ShaderStageFlagBits::eTessellationEvaluation;
        hasTessellationShaders = hasTessellationShaders || shader.type == vk::ShaderStageFlagBits::eTessellationControl;
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

    // Todo read patch control points from either pipelineRequest or from GLSLShaderSet
    createInfo.tessellationStateCreateInfo.patchControlPoints = 3;

    if ( hasTessellationShaders )
    {
        createInfo.inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::ePatchList;
        createInfo.pipelineCreateInfo.pTessellationState = &createInfo.tessellationStateCreateInfo;
    }

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
    RenderArea renderArea = createInfo.request.parentPass->getRenderArea( );

    createInfo.viewScissor.offset = vk::Offset2D { renderArea.x, renderArea.y };

    if ( renderArea.width == 0 || renderArea.height == 0 )
    {
        createInfo.viewScissor.extent = context->surfaceExtent;
    }
    else
    {
        createInfo.viewScissor.extent = vk::Extent2D { renderArea.width, renderArea.height };
    }

    createInfo.viewportStateCreateInfo.viewportCount = 1;
    createInfo.viewportStateCreateInfo.pViewports = &createInfo.parentPass->getViewport( );
    createInfo.viewportStateCreateInfo.scissorCount = 1;
    createInfo.viewportStateCreateInfo.pScissors = &createInfo.parentPass->getViewScissor( );

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
    createInfo.rasterizationStateCreateInfo.depthBiasEnable = createInfo.request.parentPass->getProperty( "DepthBiasEnabled" ) == "true";
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

        if ( createInfo.request.blendMode == BlendMode::AlphaBlend )
        {
            createInfo.colorBlendAttachments[ i ].blendEnable = true;
            createInfo.colorBlendAttachments[ i ].srcColorBlendFactor = vk::BlendFactor::eOne;
            createInfo.colorBlendAttachments[ i ].dstColorBlendFactor = vk::BlendFactor::eOne;

            createInfo.colorBlendAttachments[ i ].srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha;
            createInfo.colorBlendAttachments[ i ].dstAlphaBlendFactor = vk::BlendFactor::eDstAlpha;

            createInfo.colorBlendAttachments[ i ].colorBlendOp = vk::BlendOp::eAdd;
            createInfo.colorBlendAttachments[ i ].alphaBlendOp = vk::BlendOp::eAdd;
        }
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

void VulkanPipelineProvider::createPipelineLayout( PipelineCreateInfos &createInfo, VulkanPipeline *instance )
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
    createInfo.depthStencilStateCreateInfo.depthTestEnable = createInfo.request.enableDepthTest;
    createInfo.depthStencilStateCreateInfo.depthWriteEnable = createInfo.request.enableDepthTest;

    auto setCompareOp = [ ]( vk::CompareOp &vkCompareOp, const CompareOp &blazarCompareOp )
    {
        switch ( blazarCompareOp )
        {
            case CompareOp::Always:
                vkCompareOp = vk::CompareOp::eAlways;
                break;
            case CompareOp::Equal:
                vkCompareOp = vk::CompareOp::eEqual;
                break;
            case CompareOp::NotEqual:
                vkCompareOp = vk::CompareOp::eNotEqual;
                break;
            case CompareOp::Less:
                vkCompareOp = vk::CompareOp::eLess;
                break;
            case CompareOp::LessOrEqual:
                vkCompareOp = vk::CompareOp::eLessOrEqual;
                break;
            case CompareOp::Greater:
                vkCompareOp = vk::CompareOp::eGreater;
                break;
            case CompareOp::GreaterOrEqual:
                vkCompareOp = vk::CompareOp::eGreaterOrEqual;
                break;
        }
    };

    auto setStencilOp = [ ]( vk::StencilOp &vkStencilOp, const StencilOp &blazarStencilOp )
    {
        switch ( blazarStencilOp )
        {
            case StencilOp::Keep:
                vkStencilOp = vk::StencilOp::eKeep;
                break;
            case StencilOp::Zero:
                vkStencilOp = vk::StencilOp::eZero;
                break;
            case StencilOp::Replace:
                vkStencilOp = vk::StencilOp::eReplace;
                break;
            case StencilOp::IncrementAndClamp:
                vkStencilOp = vk::StencilOp::eIncrementAndClamp;
                break;
            case StencilOp::DecrementAndClamp:
                vkStencilOp = vk::StencilOp::eDecrementAndClamp;
                break;
            case StencilOp::Invert:
                vkStencilOp = vk::StencilOp::eInvert;
                break;
            case StencilOp::IncrementAndWrap:
                vkStencilOp = vk::StencilOp::eIncrementAndWrap;
                break;
            case StencilOp::DecrementAndWrap:
                vkStencilOp = vk::StencilOp::eDecrementAndWrap;
                break;
        }
    };

    setCompareOp( createInfo.depthStencilStateCreateInfo.depthCompareOp, createInfo.request.depthCompareOp );

    createInfo.depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    createInfo.depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

    bool enableStencilTest = createInfo.request.stencilTestStateFront.enabled || createInfo.request.stencilTestStateBack.enabled;

    createInfo.depthStencilStateCreateInfo.stencilTestEnable = enableStencilTest;

    createInfo.depthStencilStateCreateInfo.front = vk::StencilOpState { };
    createInfo.depthStencilStateCreateInfo.back = vk::StencilOpState { };

    auto initStencilState = [ = ]( vk::StencilOpState &state, const StencilTestState &blazarState )
    {
        FUNCTION_BREAK( !blazarState.enabled );

        setCompareOp( state.compareOp, blazarState.compareOp );
        state.compareMask = blazarState.compareMask;
        state.writeMask = blazarState.writeMask;
        state.reference = blazarState.ref;
        setStencilOp( state.failOp, blazarState.failOp );
        setStencilOp( state.passOp, blazarState.passOp );
        setStencilOp( state.depthFailOp, blazarState.depthFailOp );
    };

    initStencilState( createInfo.depthStencilStateCreateInfo.front, createInfo.request.stencilTestStateFront );
    initStencilState( createInfo.depthStencilStateCreateInfo.back, createInfo.request.stencilTestStateFront );

    createInfo.pipelineCreateInfo.pDepthStencilState = &createInfo.depthStencilStateCreateInfo;
}

vk::ShaderModule VulkanPipelineProvider::createShaderModule( std::vector< uint32_t > data )
{
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo { };
    shaderModuleCreateInfo.codeSize = data.size( ) * sizeof( uint32_t );
    shaderModuleCreateInfo.pCode = data.data( );

    return context->logicalDevice.createShaderModule( shaderModuleCreateInfo );
}

VulkanPipelineProvider::~VulkanPipelineProvider( )
{
    for ( auto &module: shaderModules )
    {
        context->logicalDevice.destroyShaderModule( module );
    }

    for ( auto &instance: pipelineInstances )
    {
        instance.reset( );
    }
}
END_NAMESPACES