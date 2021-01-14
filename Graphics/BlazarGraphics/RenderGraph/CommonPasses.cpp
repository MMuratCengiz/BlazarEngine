
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

std::shared_ptr< Pass > CommonPasses::createLightingPass( IRenderDevice *renderDevice )
{
    auto lightingPass = std::make_shared< Pass >( "lightingPass" );
    lightingPass->pipelineInputs.resize( 1 );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Position" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Normal" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_AlbedoSpec" );
    lightingPass->pipelineInputs[ 0 ].push_back( "gBuffer_Scene" );
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

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/quad_position.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/lighting_pass.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
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

            lightProjection = VK_CORRECTION_MATRIX * lightProjection;

            glm::vec3 pos = glm::vec3( 10.4072, 11.5711, -9.09731 );
            glm::vec3 front = glm::vec3( -0.68921, -0.48481, 0.53847 );

            glm::vec3 right = glm::cross( front, glm::vec3( 0.0f, 1.0f, 0.0f ) );
            glm::vec3 up = glm::cross( right, front );
            glm::mat4 lightView = glm::lookAt( pos, pos + front, up );

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

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/shadowMap.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/shadowMap.spv" );
    pipelineRequest.enabledPipelineStages.vertex = true;
    pipelineRequest.enabledPipelineStages.fragment = false;
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

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/cube_position.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/skybox_pass.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
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

    pipelineRequest.vertexShaderPath = PATH( "/Shaders/SPIRV/Vertex/quad_position.spv" );
    pipelineRequest.fragmentShaderPath = PATH( "/Shaders/SPIRV/Fragment/present_pass.spv" );
    pipelineRequest.enabledPipelineStages = { true, true };
    pipelineRequest.cullMode = ECS::CullMode::None;
    pipelineRequest.depthCompareOp = CompareOp::Less;

    presentPass->selectPipeline = [ ]( const std::shared_ptr< ECS::IGameEntity > &entity )
    {
        return 0;
    };

    return presentPass;
}

END_NAMESPACES
