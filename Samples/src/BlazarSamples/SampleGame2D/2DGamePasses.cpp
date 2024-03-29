#include <BlazarGraphics/RenderGraph/StaticVars.h>
#include <BlazarSamples/SampleGame2D/2DGamePasses.h>

using namespace BlazarEngine;
using namespace Graphics;

std::unique_ptr< Pass > TDGamePasses::createPresentPass( )
{
    auto presentPass = std::make_unique< Graphics::Pass >( "presentPass" );
    presentPass->inputGeometry = Graphics::InputGeometry::Model;

    auto &presentImage = presentPass->outputs.emplace_back( Graphics::OutputImage { } );
    presentImage.outputResourceName = "presentImage";
    presentImage.imageFormat = Graphics::ResourceImageFormat::MatchSwapChainImageFormat;
    presentImage.flags.msaaSampled = false;
    presentImage.flags.presentedImage = true;
    presentImage.attachmentType = Graphics::ResourceAttachmentType::Color;

    Graphics::RenderPassRequest renderPassRequest { };
    renderPassRequest.isFinalDrawPass = true;

    presentPass->renderPassRequest = renderPassRequest;

    Graphics::PipelineRequest &pipelineRequest = presentPass->pipelineRequests.emplace_back( );

    pipelineRequest.shaderPaths[ Graphics::ShaderType::Vertex ] = "./Shaders/Vertex/2d_vertex.glsl";
    pipelineRequest.shaderPaths[ Graphics::ShaderType::Fragment ] = "./Shaders/Fragment/2d_frag.glsl";
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    presentPass->selectPipeline = [ ]( ECS::IGameEntity * entity )
    {
        return std::vector< int >( 1, 0 );
    };

    return std::move( presentPass );
}