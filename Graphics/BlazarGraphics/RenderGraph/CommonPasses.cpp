
#include "CommonPasses.h"
#include "StaticVars.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< Pass > CommonPasses::createGBufferPass( IRenderDevice* renderDevice )
{
    auto gBufferPass = std::make_shared< Pass >( "gBufferPass" );
    gBufferPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    gBufferPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::Material ) );
    gBufferPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );

    gBufferPass->outputs.emplace_back( OutputImage { ResourceImageFormat::R8G8B8A8Unorm, "gBufferOutFrame" } );

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    gBufferPass->renderPassRequest = renderPassRequest;

    PipelineRequest pipelineRequest { };

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/gbuffer.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/gbuffer.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    gBufferPass->pipelineRequest = pipelineRequest;

    return gBufferPass;
}

std::shared_ptr< Pass > CommonPasses::createDefaultPass( IRenderDevice* renderDevice )
{
    auto defaultPass = std::make_shared< Pass >( "defaultPass" );
    defaultPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    defaultPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::Material ) );
    defaultPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );
    defaultPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::EnvironmentLights ) );
    defaultPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
//    defaultPass->inputs.emplace_back( StaticVars::getInputName( StaticVars::Input::SkyBox ) );

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    defaultPass->renderPassRequest = renderPassRequest;

    PipelineRequest pipelineRequest { };

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/default.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/default.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    defaultPass->pipelineRequest = pipelineRequest;

    return defaultPass;
}

END_NAMESPACES
