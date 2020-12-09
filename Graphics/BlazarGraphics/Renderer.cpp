//
// Created by Murat on 10/20/2020.
//

#include "Renderer.h"
#include <BlazarInput/GlobalEventHandler.h>
#include <BlazarECS/ECS.h>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

Renderer::Renderer( const std::shared_ptr< InstanceContext > &context, std::shared_ptr< PipelineSelector > pipelineSelector ) : context( context ), pipelineSelector( std::move( pipelineSelector ) )
{
    poolSize = context->swapChainImages.size( );

    createFrameContexts( );

    meshLoader = std::make_shared< MeshLoader >( context, frameContexts[ 0 ].commandExecutor );
    materialLoader = std::make_shared< MaterialLoader >( context, frameContexts[ 0 ].commandExecutor );
    cubeMapLoader = std::make_shared< CubeMapLoader >( context, frameContexts[ 0 ].commandExecutor );

    createSynchronizationStructures( context->logicalDevice );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::WindowResized, [ & ]( const Input::EventType &event,
                                                                                                     const Input::pEventParameters &parameters )
    {
        frameBufferResized = true;
    } );
}

void Renderer::createFrameContexts( )
{
    frameContexts.resize( poolSize, FrameContext { } );

    for ( uint32_t i = 0; i < poolSize; ++i )
    {
        FrameContext &fContext = frameContexts[ i ];

        fContext.commandExecutor = std::make_shared< CommandExecutor >( context );

        fContext.transformLoader = std::make_shared< TransformLoader >( context );
        fContext.cameraLoader = std::make_shared< CameraLoader >( context );
        fContext.lightLoader = std::make_shared< LightLoader >( context, fContext.commandExecutor );
        fContext.worldContextLoader = std::make_shared< WorldContextLoader >( context );

        for ( const PipelineInstance &instance: pipelineSelector->selectAll( ) )
        {
            instance.descriptorManager->updateUniform( i, Core::Constants::getConstant( Core::ConstantName::ShaderInputViewProjection ), fContext.cameraLoader->getBuffer( ) );
            instance.descriptorManager->updateUniform( i, Core::Constants::getConstant( Core::ConstantName::ShaderInputWorldContext ), fContext.worldContextLoader->getBuffer( ) );
        }
    }
}

void Renderer::updateTransformComponent( const pGameEntity &entity, const FrameContext &currentFrameContext, const PipelineInstance &pipeline ) const
{
    const auto &transformComponent = entity->getComponent< CTransform >( );
    const auto &material = entity->getComponent< CMaterial >( );

    FUNCTION_BREAK( IS_NULL( transformComponent ) );

    const glm::mat4 &model = TransformLoader::getModelMatrix( transformComponent );

    struct Data {
        glm::mat4 model;
        glm::vec4 scale;
    };

    glm::vec4 scale ( 1.0f );

    if ( material != nullptr )
    {
        if ( material->textureScaleOptions.scaleX )
        {
            scale.x = transformComponent->scale.x;
        }
        if ( material->textureScaleOptions.scaleX )
        {
            scale.y = transformComponent->scale.y;
        }
        if ( material->textureScaleOptions.scaleX )
        {
            scale.z = transformComponent->scale.z;
        }
    }

    Data data { model, scale };

    currentFrameContext.cachedBuffers->pushConstant(
            pipeline.layout,
            vk::ShaderStageFlagBits::eVertex,
            4 * 4 * sizeof( float ) + 4 * sizeof( float ),
            &data
    );
}

void Renderer::updateMaterialComponent( const std::shared_ptr< IGameEntity > &entity, std::shared_ptr< DescriptorManager > &manager, std::vector< vk::DescriptorSet > &setsToBind )
{
    const auto &materialComponent = entity->getComponent< CMaterial >( );

    FUNCTION_BREAK( IS_NULL( materialComponent ) );

    MaterialBuffer materialBuffer { };
    materialLoader->load( materialBuffer, *materialComponent );

    manager->updateUniform( frameIndex, Core::Constants::getConstant( Core::ConstantName::ShaderInputMaterial ), materialBuffer.buffer );

    uint32_t i = 0;
    for ( auto &part: materialBuffer.texturesObjects )
    {
        std::string &path = materialComponent->textures[ i ].path;

        if ( !manager->existsSetForTexture( frameIndex, path ) )
        {
            manager->updateTexture( frameIndex, path, part );
        }

        setsToBind.emplace_back( manager->getTextureDescriptorSet( frameIndex, path, i ) );
    }

    setsToBind.emplace_back( manager->getUniformDescriptorSet( frameIndex, Core::Constants::getConstant( Core::ConstantName::ShaderInputMaterial ) ) );
}

void Renderer::updateCubeComponent( const std::shared_ptr< IGameEntity > &entity, std::shared_ptr< DescriptorManager > &manager, std::vector< vk::DescriptorSet > &setsToBind )
{
    const auto &cubeMapComponent = entity->getComponent< CCubeMap >( );

    FUNCTION_BREAK( IS_NULL( cubeMapComponent ) );

    TextureBuffer textureBuffer { };
    cubeMapLoader->load( textureBuffer, *cubeMapComponent );

    const std::string key = cubeMapLoader->getKey( *cubeMapComponent );

    if ( !manager->existsSetForTexture( frameIndex, key ) )
    {
        manager->updateTexture( frameIndex, key, textureBuffer );
    }

    setsToBind.emplace_back( manager->getTextureDescriptorSet( frameIndex, key, 0 ) );
}

void Renderer::createSynchronizationStructures( const vk::Device &device )
{
    vk::SemaphoreCreateInfo semaphoreCreateInfo { };

    vk::FenceCreateInfo fenceCreateInfo { };
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    this->imageAvailableSemaphores.resize( this->poolSize );
    this->renderFinishedSemaphores.resize( this->poolSize );

    this->inFlightFences.resize( this->poolSize );
    this->imagesInFlight.resize( this->context->swapChainImages.size( ), nullptr );

    for ( uint32_t i = 0; i < this->imageAvailableSemaphores.size( ); ++i )
    {
        this->imageAvailableSemaphores[ i ] = device.createSemaphore( semaphoreCreateInfo );
        this->renderFinishedSemaphores[ i ] = device.createSemaphore( semaphoreCreateInfo );
        this->inFlightFences[ i ] = device.createFence( fenceCreateInfo );
    }
}


void Renderer::frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    initScene( componentTable );

    FrameContext &currentFrameContext = frameContexts[ frameIndex ];

    auto waitResult = context->logicalDevice.waitForFences( 1, &inFlightFences[ frameIndex ], VK_TRUE, UINT64_MAX );

    VkCheckResult( waitResult );

    auto result = context->logicalDevice.acquireNextImageKHR( context->swapChain, UINT64_MAX, imageAvailableSemaphores[ frameIndex ], nullptr );

    if ( result.result == vk::Result::eErrorOutOfDateKHR )
    {
        Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
        return;
    } else if ( result.result != vk::Result::eSuccess && result.result == vk::Result::eSuboptimalKHR )
    {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }

    currentFrameContext.currentActiveImage = result.value;

    currentFrameContext.cameraLoader->reload( );

    if ( currentFrameContext.cachedBuffers == nullptr )
    {
        std::shared_ptr< CommandExecutor > &currentExecutor = currentFrameContext.commandExecutor;

        currentFrameContext.cachedBuffers = currentExecutor->startCommandExecution( )
                ->generateBuffers( { }, context->frameBuffers.size( ) ); // TODO this could be problematic
    }

    currentFrameContext.lightLoader->load( );

    currentFrameContext.worldContextLoader->getWorldContext( ).worldPosition = glm::vec4( camera->position, 1.0f );
    currentFrameContext.worldContextLoader->update( );

    vk::ClearColorValue colorClear = { std::array< float, 4 > { 0.0f, 0.0f, 0.0f, 1.0f } };

    currentFrameContext.cachedBuffers->beginCommand( )->beginRenderPass( context->frameBuffers.data( ), colorClear );
}

void Renderer::entityTick( const std::shared_ptr< IGameEntity > &entity )
{
    const auto &meshComponent = entity->getComponent< CMesh >( );

    FUNCTION_BREAK( meshComponent == nullptr )

    FrameContext &currentFrameContext = frameContexts[ frameIndex ];
    std::vector< vk::Framebuffer > &frameBuffers = context->frameBuffers;

    PipelineInstance &pipeline = pipelineSelector->selectPipeline( entity );
    auto &manager = pipeline.descriptorManager;

    std::vector< vk::DescriptorSet > setsToBind;

    currentFrameContext.cachedBuffers->bindRenderPass( pipeline.pipeline, vk::PipelineBindPoint::eGraphics );
    currentFrameContext.cachedBuffers->setViewport( context->viewport )->setViewScissor( context->viewScissor );

    setsToBind.emplace_back( manager->getUniformDescriptorSet( frameIndex, Core::Constants::getConstant( Core::ConstantName::ShaderInputViewProjection ) ) );
    setsToBind.emplace_back( manager->getUniformDescriptorSet( frameIndex, Core::Constants::getConstant( Core::ConstantName::ShaderInputWorldContext ) ) );

    updateTransformComponent( entity, currentFrameContext, pipeline );
    updateMaterialComponent( entity, manager, setsToBind );
    updateCubeComponent( entity, manager, setsToBind );

    // TODO automatically order the sets to bind
    if ( pipeline.properties.supportsLighting )
    {
        setsToBind.emplace_back( manager->getUniformDescriptorSet( frameIndex, Core::Constants::getConstant( Core::ConstantName::ShaderInputEnvironmentLights ) ) );
    }

    // TODO this is might not be correct once we add particle systems
    FUNCTION_BREAK( IS_NULL( meshComponent ) )

    ObjectBufferList objectBuffer { };
    meshLoader->load( objectBuffer, *meshComponent );

    uint32_t offset = 0;
    for ( auto &part: objectBuffer.buffers )
    {
        currentFrameContext.cachedBuffers
                ->bindDescriptorSet( pipeline.layout, setsToBind )
                ->bindVertexMemory( part.vertexBuffer.first, offset )
                ->filter( part.indexCount > 0 )
                        /**/->bindIndexMemory( part.indexBuffer.first, offset )
                        /**/->drawIndexed( part.indexCount )
                ->otherwise( )
                        /**/->draw( part.vertexCount )
                ->endFilter( );
    }}

void Renderer::frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    FrameContext &currentFrameContext = frameContexts[ frameIndex ];

    currentFrameContext.cachedBuffers->endRenderPass( )->execute( );

    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo { };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[ frameIndex ];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentFrameContext.cachedBuffers->getBuffers( )[ currentFrameContext.currentActiveImage ];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores[ frameIndex ];

    context->logicalDevice.resetFences( 1, &inFlightFences[ frameIndex ] );

    auto submitResult = context->queues[ QueueType::Graphics ].submit( 1, &submitInfo, inFlightFences[ frameIndex ] );

    VkCheckResult( submitResult );

    vk::PresentInfoKHR presentInfo { };

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[ frameIndex ];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context->swapChain;
    presentInfo.pImageIndices = &currentFrameContext.currentActiveImage;
    presentInfo.pResults = nullptr;

    auto presentResult = context->queues[ QueueType::Presentation ].presentKHR( presentInfo );

    if ( presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR || frameBufferResized )
    {
        frameBufferResized = false;
        Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
        return;
    } else if ( presentResult != vk::Result::eSuccess )
    {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }

    frameIndex = ( frameIndex + 2 ) & poolSize;
}

void Renderer::freeBuffers( )
{
    for ( FrameContext &fc: frameContexts )
    {
        fc.cachedBuffers.reset( );
        fc.cachedBuffers = nullptr;
    }
}

Renderer::~Renderer( )
{
    for ( uint32_t index = 0; index < imageAvailableSemaphores.size( ); ++index )
    {
        context->logicalDevice.destroySemaphore( renderFinishedSemaphores[ index ], nullptr );
        context->logicalDevice.destroySemaphore( imageAvailableSemaphores[ index ], nullptr );
        context->logicalDevice.destroyFence( inFlightFences[ index ], nullptr );
    }
}

void Renderer::initScene( const std::shared_ptr< ECS::ComponentTable >& componentTable )
{
    FUNCTION_BREAK( sceneInitialized )
    sceneInitialized = true;

    for ( const auto &cCamera: componentTable->getComponents< ECS::CCamera >( ) )
    {
        // TODO maybe should be combine the cameras somehow
        this->camera = cCamera;
    }

    for ( uint32_t i = 0; i < poolSize; ++i )
    {
        FrameContext &fContext = frameContexts[ i ];

        fContext.cameraLoader->reload( this->camera );

        for ( const auto &ambientLight: componentTable->getComponents< ECS::CAmbientLight >( ) )
        {
            fContext.lightLoader->addAmbientLight( ambientLight );
        }

        for ( const auto &directionLight: componentTable->getComponents< ECS::CDirectionalLight >( ) )
        {
            fContext.lightLoader->addDirectionalLight( directionLight );
        }

        for ( const auto &pointLight: componentTable->getComponents< ECS::CPointLight >( ) )
        {
            fContext.lightLoader->addPointLight( pointLight );
        }

        for ( const auto &spotLight: componentTable->getComponents< ECS::CSpotLight >( )  )
        {
            fContext.lightLoader->addSpotLight( spotLight );
        }

        fContext.lightLoader->load( );

        for ( const PipelineInstance &instance: pipelineSelector->selectAll( ) )
        {
            instance.descriptorManager->updateUniform( i, Core::Constants::getConstant( Core::ConstantName::ShaderInputEnvironmentLights ), fContext.lightLoader->getBuffer( ) );
        }
    }
}

END_NAMESPACES