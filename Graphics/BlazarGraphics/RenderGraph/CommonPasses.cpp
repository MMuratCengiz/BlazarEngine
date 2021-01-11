
#include "CommonPasses.h"
#include "StaticVars.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

std::shared_ptr< Pass > CommonPasses::createGBufferPass( IRenderDevice *renderDevice )
{
    auto gBufferPass = std::make_shared< Pass >( "gBufferPass" );
    gBufferPass->pipelineInputs.resize( 1 );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::Material ) );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ViewProjection ) );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
    gBufferPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) );

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

    auto &gBuffer_Scene = gBufferPass->outputs.emplace_back( OutputImage { } );
    gBuffer_Scene.outputResourceName = "gBuffer_Scene";
    gBuffer_Scene.imageFormat = ResourceImageFormat::R32G32B32A32Sfloat;
    gBuffer_Scene.attachmentType = ResourceAttachmentType::Color;

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };

    gBufferPass->renderPassRequest = renderPassRequest;

    gBufferPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    PipelineRequest &pipelineRequest = gBufferPass->pipelineRequests.emplace_back( PipelineRequest { } );

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/gBuffer.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/gBuffer.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::BackFace;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    return gBufferPass;
}

std::shared_ptr< Pass > CommonPasses::createDefaultPass( IRenderDevice *renderDevice )
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

    auto &depthBuffer = defaultPass->outputs.emplace_back( );
    depthBuffer.outputResourceName = "depthBuffer";
    depthBuffer.imageFormat = ResourceImageFormat::BestDepthFormat;
    depthBuffer.flags.msaaSampled = true;
    depthBuffer.attachmentType = ResourceAttachmentType::Depth;

    auto &defaultPass_Result = defaultPass->outputs.emplace_back( OutputImage { } );
    defaultPass_Result.outputResourceName = "defaultPass_Result";
    defaultPass_Result.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    defaultPass_Result.flags.msaaSampled = true;
    defaultPass_Result.attachmentType = ResourceAttachmentType::Color;

    auto &defaultPass_ResultRed = defaultPass->outputs.emplace_back( OutputImage { } );
    defaultPass_ResultRed.outputResourceName = "defaultPass_ResultRed";
    defaultPass_ResultRed.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    defaultPass_ResultRed.flags.msaaSampled = true;
    defaultPass_ResultRed.attachmentType = ResourceAttachmentType::Color;

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
    cubeMapPipelineRequest.cullMode = ECS::CullMode::None;
    cubeMapPipelineRequest.depthCompareOp = CompareOp::LessOrEqual;

    defaultPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return entity->hasComponent< ECS::CCubeMap >( ) ? 1 : 0;
    };

    return defaultPass;
}

std::shared_ptr< Pass > CommonPasses::createFinalDrawPass( IRenderDevice *renderDevice )
{
    auto finalDrawPass = std::make_shared< Pass >( "defaultFinal" );
    finalDrawPass->pipelineInputs.resize( 1 );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "gBuffer_Position" );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "gBuffer_Normal" );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "gBuffer_AlbedoSpec" );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "gBuffer_Scene" );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "shadowMap" );
    finalDrawPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::EnvironmentLights ) );
    finalDrawPass->pipelineInputs[ 0 ].push_back( "LightViewProjectionMatrix" );

    auto &presentImage = finalDrawPass->outputs.emplace_back( OutputImage { } );
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
    pipelineRequest.cullMode = ECS::CullMode::FrontFace;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    finalDrawPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return finalDrawPass;
}

std::shared_ptr< Pass > CommonPasses::createShadowMapPass( IRenderDevice *renderDevice )
{
    auto shadowMapPass = std::make_shared< Pass >( "shadowMap" );
    shadowMapPass->pipelineInputs.resize( 1 );
    shadowMapPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::GeometryData ) );
    shadowMapPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::ModelMatrix ) );
#ifdef ENABLE_SHADOW_DEBUG_OUTPUT
    shadowMapPass->pipelineInputs[ 0 ].push_back( StaticVars::getInputName( StaticVars::Input::Material ) );
#endif
    shadowMapPass->pipelineInputs[ 0 ].push_back( "LightViewProjectionMatrix" );

    shadowMapPass->customFormatters[ "LightViewProjectionMatrix" ] = [ ]( const std::shared_ptr< ECS::ComponentTable > &components )
    {
        AttachmentContent content { };

        const auto directionalLights = components->getComponents< ECS::CDirectionalLight >( );
        const auto cameras = components->getComponents< ECS::CCamera >( );

        std::shared_ptr< ECS::CCamera > activeCamera;

        for ( const auto &camera : cameras )
        {
            if ( camera->isActive )
            {
                activeCamera = camera;
                break;
            }
        }

        size_t mat4Size = 4 * 4 * sizeof( float );

        uint32_t arraySize = 3;

        content.size = 3 * mat4Size + sizeof( int );
        content.data = ( char * ) malloc( content.size );

        uint32_t offset = 0;

        for ( const auto &light: directionalLights )
        {
            glm::mat4 lightProjection = glm::ortho( -10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f );
//            lightProjection = glm::perspective( glm::radians( 60.0f ), 800.0f / 600.0f, 0.1f, 100.0f );

            lightProjection = VK_CORRECTION_MATRIX * lightProjection;

            glm::vec3 pos = glm::vec3( 10.4072, 11.5711, -9.09731 );
            glm::vec3 front = glm::vec3( -0.68921, -0.48481, 0.53847 );

            glm::vec3 right = glm::cross( front, glm::vec3( 0.0f, 1.0f, 0.0f ) );
            glm::vec3 up = glm::cross( right, front );
            glm::mat4 lightView = glm::lookAt( pos, pos + front, up );

/*            glm::mat4 lightView = glm::lookAt(glm::vec3( 4.0f,  2.0f, 10.0f ),
                                              light->direction - glm::vec3( 4.0f,  2.0f, 10.0f ),
                                              glm::vec3( 0.0f, 1.0f,  0.0f));*/

//            glm::mat4 lightView = glm::lookAt( glm::vec3( 0.0f, 3.0f, 0.0f ), activeCamera->position, glm::vec3( 0.0f, 1.0f, 0.0f ) );

            glm::mat4 result = lightProjection * lightView;

            memcpy( content.data + offset, &result, mat4Size );
            offset += mat4Size;

            if ( offset == content.size )
            {
                break;
            }
        }

        memcpy( content.data + ( content.size - sizeof( int ) ), &arraySize, sizeof( int ) );

        return content;
    };

    auto &shadowMap = shadowMapPass->outputs.emplace_back( OutputImage { } );
    shadowMap.outputResourceName = "shadowMap";
    shadowMap.imageFormat = ResourceImageFormat::BestDepthFormat;
    shadowMap.attachmentType = ResourceAttachmentType::Depth;

#ifdef ENABLE_SHADOW_DEBUG_OUTPUT
    auto &shadowMap_RGB = shadowMapPass->outputs.emplace_back( OutputImage { } );
    shadowMap_RGB.outputResourceName = "shadowMap_RGB";
    shadowMap_RGB.flags.presentedImage = true;
    shadowMap_RGB.imageFormat = ResourceImageFormat::MatchSwapChainImageFormat;
    shadowMap_RGB.attachmentType = ResourceAttachmentType::Color;
#endif

    auto &pipelineProvider = renderDevice->getPipelineProvider( );
    auto &renderPassProvider = renderDevice->getRenderPassProvider( );

    RenderPassRequest renderPassRequest { };
    renderPassRequest.setDepthBias = true;
    renderPassRequest.depthBiasConstant = 1.25f;
    renderPassRequest.depthBiasSlope = 1.75f;
    renderPassRequest.dependencySet = DependencySet::ShadowMap;

    shadowMapPass->renderPassRequest = renderPassRequest;

#ifndef ENABLE_SHADOW_DEBUG_OUTPUT
    shadowMapPass->renderPassRequest.renderArea = { 0, 0, 2048, 2048 };
#endif

    PipelineRequest &pipelineRequest = shadowMapPass->pipelineRequests.emplace_back( );

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/shadowMap.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/shadowMap.spv" );
    pipelineRequest.enabledPipelineStages.vertex = true;
    pipelineRequest.enabledPipelineStages.fragment = false;
#ifdef ENABLE_SHADOW_DEBUG_OUTPUT
    pipelineRequest.enabledPipelineStages.fragment = true;
#endif

    pipelineRequest.cullMode = ECS::CullMode::FrontFace;
    pipelineRequest.depthCompareOp = CompareOp::LessOrEqual;

    shadowMapPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return shadowMapPass;
}

END_NAMESPACES
