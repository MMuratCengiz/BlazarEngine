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


#include <BlazarECS/CAnimState.h>
#include "CommonPasses.h"
#include "StaticVars.h"

#define RETURN_SINGLE_PIPELINE( value ) std::vector< int > v; v.push_back( value ); return v;

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::unique_ptr< Pass > CommonPasses::createGBufferPass( IRenderDevice *renderDevice )
{
    auto gBufferPass = std::make_unique< Pass >( "gBufferPass" );
    gBufferPass->inputGeometry = InputGeometry::Model;

    gBufferPass->pipelineInputs.resize( 5 );

    for ( int i = 0; i < 5; ++i )
    {
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) );
        gBufferPass->pipelineInputs[ i ].push_back( "InstanceData" );

        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::Material ) );
        gBufferPass->pipelineInputs[ i ].push_back( "Texture1" );

        if ( i == 1 )
        {
            gBufferPass->pipelineInputs[ i ].push_back( "HeightMap" );
            gBufferPass->pipelineInputs[ i ].push_back( "Tessellation" );
        }

        if ( i == 3 )
        {
            gBufferPass->pipelineInputs[ i ].push_back( "OutlineScale" );
            gBufferPass->pipelineInputs[ i ].push_back( "OutlineColor" );
        }

        if ( i == 4 )
        {
            gBufferPass->pipelineInputs[ i ].push_back( "BoneTransformations" );
        }
    }

    auto &depthBuffer = gBufferPass->outputs.emplace_back( );
    depthBuffer.outputResourceName = "depthBuffer";
    depthBuffer.imageFormat = ResourceImageFormat::BestDepthFormat;
    depthBuffer.attachmentType = ResourceAttachmentType::DepthAndStencil;

    auto &gBuffer_Position = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Position.outputResourceName = "gBuffer_Position";
    gBuffer_Position.imageFormat = ResourceImageFormat::R32G32B32A32Sfloat;
    gBuffer_Position.attachmentType = ResourceAttachmentType::Color;

    auto &gBuffer_Normal = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Normal.outputResourceName = "gBuffer_Normal";
    gBuffer_Normal.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    gBuffer_Normal.attachmentType = ResourceAttachmentType::Color;

    auto &gBuffer_Albedo = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Albedo.outputResourceName = "gBuffer_Albedo";
    gBuffer_Albedo.imageFormat = ResourceImageFormat::R8G8B8A8Unorm;
    gBuffer_Albedo.attachmentType = ResourceAttachmentType::Color;

    auto &gBuffer_Material = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Material.outputResourceName = "gBuffer_Material";
    gBuffer_Material.imageFormat = ResourceImageFormat::R8G8B8A8Unorm;
    gBuffer_Material.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    gBufferPass->renderPassRequest = renderPassRequest;

    gBufferPass->selectPipeline = [ ](  ECS::IGameEntity * entity )
    {
        int pipeline = entity->hasComponent< ECS::CTessellation >( ) ? 1 : 0;

        if ( entity->hasComponent< ECS::CAnimState >( ) )
        {
            RETURN_SINGLE_PIPELINE( 4 )
        }

        if ( entity->hasComponent< ECS::COutlined >( ) )
        {
            std::vector< int > pipelines;
            pipelines.push_back( 3 );
            pipelines.push_back( 2 );
            return pipelines;
        }

        RETURN_SINGLE_PIPELINE( pipeline )
    };

    auto setPipelineStencilDefaults = [ ]( PipelineRequest &request )
    {
        request.stencilTestStateFront.compareMask = 0xFF;
        request.stencilTestStateFront.writeMask = 0xFF;
        request.stencilTestStateFront.compareOp = CompareOp::Always;
        request.stencilTestStateFront.enabled = true;
        request.stencilTestStateFront.ref = 1;
        request.stencilTestStateFront.failOp = StencilOp::Replace;
        request.stencilTestStateFront.depthFailOp = StencilOp::Replace;
        request.stencilTestStateFront.passOp = StencilOp::Replace;
    };

    PipelineRequest &pipelineRequest = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    PipelineRequest &heightmapTessellationPipeline = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    heightmapTessellationPipeline.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer.spv" );
    heightmapTessellationPipeline.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    heightmapTessellationPipeline.shaderPaths[ ShaderType::TessellationControl ] = PATH( "/Shaders/SPIRV/tesscontrol/height_map.spv" );
    heightmapTessellationPipeline.shaderPaths[ ShaderType::TessellationEval ] = PATH( "/Shaders/SPIRV/tesseval/height_map.spv" );
    heightmapTessellationPipeline.cullMode = ECS::CullMode::None;
    heightmapTessellationPipeline.depthCompareOp = CompareOp::Less;

    PipelineRequest &stencilTestEnabled = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    stencilTestEnabled.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer.spv" );
    stencilTestEnabled.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    stencilTestEnabled.cullMode = ECS::CullMode::None;
    stencilTestEnabled.depthCompareOp = CompareOp::Less;
    setPipelineStencilDefaults( stencilTestEnabled );

    PipelineRequest &outlinedPipeline = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    outlinedPipeline.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer_outlined.spv" );
    outlinedPipeline.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer_outlined.spv" );
    outlinedPipeline.cullMode = ECS::CullMode::None;
    outlinedPipeline.depthCompareOp = CompareOp::Less;
    outlinedPipeline.enableDepthTest = false;

    setPipelineStencilDefaults( outlinedPipeline );
    outlinedPipeline.stencilTestStateFront.compareOp = CompareOp::NotEqual;
    outlinedPipeline.stencilTestStateFront.failOp = StencilOp::Keep;
    outlinedPipeline.stencilTestStateFront.depthFailOp = StencilOp::Keep;
    outlinedPipeline.stencilTestStateFront.passOp = StencilOp::Replace;

    PipelineRequest &animatedGBufferRequest = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    animatedGBufferRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer_Animated.spv" );
    animatedGBufferRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    animatedGBufferRequest.cullMode = ECS::CullMode::None;
    animatedGBufferRequest.depthCompareOp = CompareOp::Less;

    return std::move( gBufferPass );
}

std::unique_ptr< Pass > CommonPasses::createLightingPass( IRenderDevice *renderDevice )
{
    auto lightingPass = std::make_unique< Pass >( "lightingPass" );
    lightingPass->inputGeometry = InputGeometry::Quad;

    lightingPass->pipelineInputs.resize( 1 );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Position" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Normal" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Albedo" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Material" );
    lightingPass->pipelineInputs[ 0 ].push_back( "WorldContext" );
    lightingPass->pipelineInputs[ 0 ].push_back( "shadowMap" );
    lightingPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::EnvironmentLights ) );
    lightingPass->pipelineInputs[ 0 ].push_back( "LightViewProjectionMatrix" );

    auto &presentImage = lightingPass->outputs.emplace_back( OutputImage { } );
    presentImage.outputResourceName = "litScene";
    presentImage.imageFormat = ResourceImageFormat::R16G16B16A16Sfloat;
    presentImage.flags.msaaSampled = false;
    presentImage.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    lightingPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = lightingPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/quad_position.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/lighting_pass.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    lightingPass->selectPipeline = [ ](  ECS::IGameEntity * entity )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( lightingPass );
}

std::unique_ptr< Pass > CommonPasses::createShadowMapPass( IRenderDevice *renderDevice )
{
    auto shadowMapPass = std::make_unique< Pass >( "shadowMap" );
    shadowMapPass->inputGeometry = InputGeometry::Model;

    shadowMapPass->pipelineInputs.resize( 1 );
    shadowMapPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
    shadowMapPass->pipelineInputs[ 0 ].push_back( "InstanceData" );
    shadowMapPass->pipelineInputs[ 0 ].push_back( "LightViewProjectionMatrix" );

    auto &shadowMap = shadowMapPass->outputs.emplace_back( OutputImage { } );
    shadowMap.outputResourceName = "shadowMap";
    shadowMap.imageFormat = ResourceImageFormat::BestDepthFormat;
    shadowMap.attachmentType = ResourceAttachmentType::Depth;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };
    renderPassRequest.setDepthBias = true;
    renderPassRequest.depthBiasConstant = 1.25f;
    renderPassRequest.depthBiasSlope = 1.75f;
    renderPassRequest.dependencySet = DependencySet::ShadowMap;

    shadowMapPass->renderPassRequest = renderPassRequest;
    shadowMapPass->renderPassRequest.renderArea = { 0, 0, 4096, 4096 };

    PipelineRequest &pipelineRequest = shadowMapPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/shadowMap.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::LessOrEqual;

    shadowMapPass->selectPipeline = [ ](  ECS::IGameEntity * entity )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( shadowMapPass );
}

std::unique_ptr< Pass > CommonPasses::createSkyBoxPass( IRenderDevice *renderDevice )
{
    auto skyboxPass = std::make_unique< Pass >( "skyBoxPass" );
    skyboxPass->inputGeometry = InputGeometry::Cube;

    skyboxPass->pipelineInputs.resize( 1 );
    skyboxPass->pipelineInputs[ 0 ].push_back( "ViewProjection" );
    skyboxPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::SkyBox ) );

    auto &presentImage = skyboxPass->outputs.emplace_back( OutputImage { } );
    presentImage.outputResourceName = "skyBoxTex";
    presentImage.imageFormat = ResourceImageFormat::R16G16B16A16Sfloat;
    presentImage.flags.msaaSampled = false;
    presentImage.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    skyboxPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = skyboxPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/cube_position.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/skybox_pass.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    skyboxPass->selectPipeline = [ ](  ECS::IGameEntity * entity )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( skyboxPass );
}

std::unique_ptr< Pass > CommonPasses::createSMAAEdgePass( IRenderDevice *renderDevice )
{
    auto smaaEdgePass = std::make_unique< Pass >( "smaaEdgePass" );
    smaaEdgePass->inputGeometry = InputGeometry::OverSizedTriangle;

    smaaEdgePass->pipelineInputs.resize( 1 );
    smaaEdgePass->pipelineInputs[ 0 ].push_back( "Resolution" );
    smaaEdgePass->pipelineInputs[ 0 ].push_back( "litScene" );

    auto &edgesTex = smaaEdgePass->outputs.emplace_back( OutputImage { } );
    edgesTex.outputResourceName = "edgesTex";
    edgesTex.imageFormat = ResourceImageFormat::R32G32B32A32Sfloat;
    edgesTex.flags.msaaSampled = false;
    edgesTex.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    smaaEdgePass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = smaaEdgePass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/smaaEdge.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/smaaEdge.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    smaaEdgePass->selectPipeline = [ ](  ECS::IGameEntity * entity )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( smaaEdgePass );
}

std::unique_ptr< Pass > CommonPasses::createSMAABlendWeightPass( IRenderDevice *renderDevice )
{
    auto smaaBlendWeightPass = std::make_unique< Pass >( "smaaBlendWeightPass" );
    smaaBlendWeightPass->inputGeometry = InputGeometry::OverSizedTriangle;

    smaaBlendWeightPass->pipelineInputs.resize( 1 );
    smaaBlendWeightPass->pipelineInputs[ 0 ].push_back( "Resolution" );
    smaaBlendWeightPass->pipelineInputs[ 0 ].push_back( "edgesTex" );
    smaaBlendWeightPass->pipelineInputs[ 0 ].push_back( "areaTex" );
    smaaBlendWeightPass->pipelineInputs[ 0 ].push_back( "searchTex" );

    auto &blendTex = smaaBlendWeightPass->outputs.emplace_back( OutputImage { } );
    blendTex.outputResourceName = "blendTex";
    blendTex.imageFormat = ResourceImageFormat::R32G32B32A32Sfloat;
    blendTex.flags.msaaSampled = false;
    blendTex.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    smaaBlendWeightPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = smaaBlendWeightPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/smaaBlendWeight.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/smaaBlendWeight.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    smaaBlendWeightPass->selectPipeline = [ ](  ECS::IGameEntity * entity )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( smaaBlendWeightPass );
}

std::unique_ptr< Pass > CommonPasses::createSMAANeighborPass( IRenderDevice *renderDevice )
{
    auto smaaNeighborPass = std::make_unique< Pass >( "smaaNeighborPass" );
    smaaNeighborPass->inputGeometry = InputGeometry::OverSizedTriangle;

    smaaNeighborPass->pipelineInputs.resize( 1 );
    smaaNeighborPass->pipelineInputs[ 0 ].push_back( "Resolution" );
    smaaNeighborPass->pipelineInputs[ 0 ].push_back( "litScene" );
    smaaNeighborPass->pipelineInputs[ 0 ].push_back( "blendTex" );

    auto &aliasedImage = smaaNeighborPass->outputs.emplace_back( OutputImage { } );
    aliasedImage.outputResourceName = "aliasedImage";
    aliasedImage.imageFormat = ResourceImageFormat::R32G32B32A32Sfloat;
    aliasedImage.flags.msaaSampled = false;
    aliasedImage.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    smaaNeighborPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = smaaNeighborPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/smaaNeighbor.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/smaaNeighbor.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    smaaNeighborPass->selectPipeline = [ ]( ECS::IGameEntity * )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( smaaNeighborPass );
}

std::unique_ptr< Pass > CommonPasses::createPresentPass( IRenderDevice *renderDevice )
{
    auto presentPass = std::make_unique< Pass >( "presentPass" );
    presentPass->inputGeometry = InputGeometry::Quad;

    presentPass->pipelineInputs.resize( 1 );
    presentPass->pipelineInputs[ 0 ].push_back( "litScene" );
    presentPass->pipelineInputs[ 0 ].push_back( "skyBoxTex" );

    auto &presentImage = presentPass->outputs.emplace_back( OutputImage { } );
    presentImage.outputResourceName = "presentImage";
    presentImage.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    presentImage.flags.msaaSampled = false;
    presentImage.flags.presentedImage = true;
    presentImage.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };
    renderPassRequest.isFinalDrawPass = true;

    presentPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = presentPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/quad_position.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/present_pass.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    presentPass->selectPipeline = [ ]( ECS::IGameEntity * entity )
    {
        RETURN_SINGLE_PIPELINE( 0 )
    };

    return std::move( presentPass );
}

END_NAMESPACES
