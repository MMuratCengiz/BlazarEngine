
#include "CommonPasses.h"
#include "StaticVars.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< Pass > CommonPasses::createGBufferPass( IRenderDevice *renderDevice )
{
    auto gBufferPass = std::make_shared< Pass >( "gBufferPass" );
    gBufferPass->pipelineInputs.resize( 2 );
    for ( int i = 0; i < 2; ++i )
    {
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) );

        gBufferPass->pipelineInputs[ i ].push_back( StaticVars::getInputName( StaticVars::Input::Material ) );
        gBufferPass->pipelineInputs[ i ].push_back( "Texture1" );
        if ( i == 1 )
        {
            gBufferPass->pipelineInputs[ i ].push_back( "HeightMap" );
            gBufferPass->pipelineInputs[ i ].push_back( "Tessellation" );
        }
    }

    auto &depthBuffer = gBufferPass->outputs.emplace_back( );
    depthBuffer.outputResourceName = "depthBuffer";
    depthBuffer.imageFormat = ResourceImageFormat::BestDepthFormat;
    depthBuffer.attachmentType = ResourceAttachmentType::Depth;

    auto &gBuffer_Position = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Position.outputResourceName = "gBuffer_Position";
    gBuffer_Position.imageFormat = ResourceImageFormat::R32G32B32A32Sfloat;
    gBuffer_Position.attachmentType = ResourceAttachmentType::Color;

    auto &gBuffer_Normal = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Normal.outputResourceName = "gBuffer_Normal";
    gBuffer_Normal.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    gBuffer_Normal.attachmentType = ResourceAttachmentType::Color;

    auto &gBuffer_AlbedoSpec = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_AlbedoSpec.outputResourceName = "gBuffer_AlbedoSpec";
    gBuffer_AlbedoSpec.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    gBuffer_AlbedoSpec.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    gBufferPass->renderPassRequest = renderPassRequest;

    gBufferPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return entity->hasComponent< ECS::CTessellation >( ) ? 1 : 0;
    };

    PipelineRequest &pipelineRequest = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    pipelineRequest.cullMode = ECS::CullMode::BackFace;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    PipelineRequest &heightmapTessellationPipeline = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    heightmapTessellationPipeline.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/gBuffer.spv" );
    heightmapTessellationPipeline.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    heightmapTessellationPipeline.shaderPaths[ ShaderType::TessellationControl ] = PATH( "/Shaders/SPIRV/tesscontrol/height_map.spv" );
    heightmapTessellationPipeline.shaderPaths[ ShaderType::TessellationEval ] = PATH( "/Shaders/SPIRV/tesseval/height_map.spv" );
    heightmapTessellationPipeline.cullMode = ECS::CullMode::BackFace;
    heightmapTessellationPipeline.depthCompareOp = CompareOp::Less;

    return gBufferPass;
}

std::shared_ptr< Pass > CommonPasses::createLightingPass( IRenderDevice *renderDevice )
{
    auto lightingPass = std::make_shared< Pass >( "lightingPass" );
    lightingPass->pipelineInputs.resize( 1 );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Position" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Normal" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_AlbedoSpec" );
    lightingPass->pipelineInputs[ 0 ].push_back( "shadowMap" );
    lightingPass->pipelineInputs[ 0 ].push_back( "ScreenQuad" );
    lightingPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::EnvironmentLights ) );
    lightingPass->pipelineInputs[ 0 ].push_back( "LightViewProjectionMatrix" );

    auto &presentImage = lightingPass->outputs.emplace_back( OutputImage { } );
    presentImage.outputResourceName = "litScene";
    presentImage.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    presentImage.flags.msaaSampled = false;
    presentImage.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    lightingPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = lightingPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/quad_position.spv" );
    pipelineRequest.shaderPaths[ ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/lighting_pass.spv" );
    pipelineRequest.cullMode = ECS::CullMode::FrontFace;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    lightingPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return lightingPass;
}

std::shared_ptr< Pass > CommonPasses::createShadowMapPass( IRenderDevice *renderDevice )
{
    auto shadowMapPass = std::make_shared< Pass >( "shadowMap" );
    shadowMapPass->pipelineInputs.resize( 1 );
    shadowMapPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    shadowMapPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
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
    shadowMapPass->renderPassRequest.renderArea = { 0, 0, 2048, 2048 };

    PipelineRequest &pipelineRequest = shadowMapPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/shadowMap.spv" );
    pipelineRequest.cullMode = ECS::CullMode::FrontFace;
    pipelineRequest.depthCompareOp = CompareOp::LessOrEqual;

    shadowMapPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return shadowMapPass;
}

std::shared_ptr< Pass > CommonPasses::createSkyBoxPass( IRenderDevice *renderDevice )
{
    auto skyboxPass = std::make_shared< Pass >( "skyBoxPass" );
    skyboxPass->pipelineInputs.resize( 1 );
    skyboxPass->pipelineInputs[ 0 ].push_back( "ViewProjection" );
    skyboxPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::SkyBox ) );
    skyboxPass->pipelineInputs[ 0 ].push_back( "ScreenCube" );

    auto &presentImage = skyboxPass->outputs.emplace_back( OutputImage { } );
    presentImage.outputResourceName = "skyBoxTex";
    presentImage.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
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

    skyboxPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return skyboxPass;
}

std::shared_ptr< Pass > CommonPasses::createPresentPass( IRenderDevice *renderDevice )
{
    auto presentPass = std::make_shared< Pass >( "presentPass" );
    presentPass->pipelineInputs.resize( 1 );
    presentPass->pipelineInputs[ 0 ].push_back( "litScene" );
    presentPass->pipelineInputs[ 0 ].push_back( "skyBoxTex" );
    presentPass->pipelineInputs[ 0 ].push_back( "ScreenQuad" );

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

    presentPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return presentPass;
}

END_NAMESPACES
