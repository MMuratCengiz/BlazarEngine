
#include "CommonPasses.h"
#include "StaticVars.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< Pass > CommonPasses::createGBufferPass( IRenderDevice* renderDevice )
{
    auto gBufferPass = std::make_shared< Pass >( "gBufferPass" );
    gBufferPass->pipelineInputs.resize( 1 );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::Material ) );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );

//    gBufferPass->outputs.emplace_back( OutputImage { ResourceImageFormat::R8G8B8A8Unorm, "gBufferOutFrame" } );

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    gBufferPass->renderPassRequest = renderPassRequest;

    gBufferPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity >& entity )
    {
        return 0;
    };

    PipelineRequest &pipelineRequest = gBufferPass->pipelineRequests.emplace_back( PipelineRequest{ } );

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/gbuffer.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/gbuffer.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    return gBufferPass;
}

std::shared_ptr< Pass > CommonPasses::createDefaultPass( IRenderDevice* renderDevice )
{
    auto defaultPass = std::make_shared< Pass >( "defaultPass" );
    defaultPass->pipelineInputs.resize( 2 );
    defaultPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    defaultPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::Material ) );
    defaultPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );
    defaultPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::EnvironmentLights ) );
    defaultPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );

    defaultPass->pipelineInputs[ 1 ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    defaultPass->pipelineInputs[ 1 ].push_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );
    defaultPass->pipelineInputs[ 1 ].push_back( StaticVars::getInputName( StaticVars::Input::SkyBox ) );

    auto & depthBuffer = defaultPass->outputs.emplace_back( );
    depthBuffer.outputResourceName = "depthBuffer";
    depthBuffer.imageFormat = ResourceImageFormat::BestDepthFormat;
    depthBuffer.flags.msaaSampled = true;
    depthBuffer.attachmentType = ResourceAttachmentType::Depth;

    auto & defaultPass_Result = defaultPass->outputs.emplace_back( OutputImage{ } );
    defaultPass_Result.outputResourceName = "defaultPass_Result";
    defaultPass_Result.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    defaultPass_Result.flags.msaaSampled = true;
    defaultPass_Result.attachmentType = ResourceAttachmentType::Color;

    auto & defaultPass_ResultRed = defaultPass->outputs.emplace_back( OutputImage{ } );
    defaultPass_ResultRed.outputResourceName = "defaultPass_ResultRed";
    defaultPass_ResultRed.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    defaultPass_ResultRed.flags.msaaSampled = true;
    defaultPass_ResultRed.attachmentType = ResourceAttachmentType::Color;

    defaultPass->outputGeometry = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };
    renderPassRequest.outputImages = defaultPass->outputs;

    defaultPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = defaultPass->pipelineRequests.emplace_back( );

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/default.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/default.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    PipelineRequest &cubeMapPipelineRequest = defaultPass->pipelineRequests.emplace_back( );

    cubeMapPipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/skybox_default.spv" );
    cubeMapPipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/skybox_default.spv" );
    cubeMapPipelineRequest.enabledPipelineStages = { true, true };
    cubeMapPipelineRequest.cullMode = ECS::CullMode::FrontFace;
    cubeMapPipelineRequest.depthCompareOp = CompareOp::LessOrEqual;

    defaultPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity >& entity )
    {
        return entity->hasComponent< ECS::CCubeMap >( ) ? 1 : 0;
    };

    return defaultPass;
}

std::shared_ptr< Pass > CommonPasses::createFinalDrawPass( IRenderDevice *renderDevice )
{
    auto finalDrawPass = std::make_shared< Pass >( "defaultFinal" );
    finalDrawPass->pipelineInputs.resize( 1 );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "defaultPass_Result" );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "defaultPass_ResultRed" );

    auto & presentImage = finalDrawPass->outputs.emplace_back( OutputImage { } );
    presentImage.outputResourceName = "presentImage";
    presentImage.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    presentImage.flags.msaaSampled = false;
    presentImage.flags.presentedImage = true;
    presentImage.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };
    renderPassRequest.isFinalDrawPass = true;

    finalDrawPass->renderPassRequest = renderPassRequest;

    PipelineRequest &pipelineRequest = finalDrawPass->pipelineRequests.emplace_back( );

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/default_final.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/default_final.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    finalDrawPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity >& entity )
    {
        return 0;
    };

    return finalDrawPass;
}

END_NAMESPACES
