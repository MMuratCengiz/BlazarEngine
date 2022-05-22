#include <BlazarGraphics/RenderGraph/StaticVars.h>
#include "2DGamePasses.h"

using namespace BlazarEngine;
using namespace Graphics;

std::unique_ptr< Pass > TDGamePasses::createPresentPass( )
{
    auto presentPass = std::make_unique< Graphics::Pass >( "presentPass" );
    presentPass->inputGeometry = Graphics::InputGeometry::Model;

    presentPass->pipelineInputs.resize( 1 );
    presentPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::ShaderInput::ViewProjection ) );
    presentPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::ShaderInput::ModelMatrix ) );
    presentPass->pipelineInputs[ 0 ].push_back( "InstanceData" );
    presentPass->pipelineInputs[ 0 ].push_back( "Texture1" );

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

    pipelineRequest.shaderPaths[ Graphics::ShaderType::Vertex ] = PATH( "/Shaders/SPIRV/Vertex/2d_vertex.spv" );
    pipelineRequest.shaderPaths[ Graphics::ShaderType::Fragment ] = PATH( "/Shaders/SPIRV/Fragment/2d_frag.spv" );
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    presentPass->selectPipeline = [ ]( ECS::IGameEntity * entity )
    {
        return std::vector< int >( 1, 0 );
    };

    return std::move( presentPass );
}