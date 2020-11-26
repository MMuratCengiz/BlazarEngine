//
// Created by Murat on 10/20/2020.
//

#include "Renderer.h"
#include "../Input/GlobalEventHandler.h"
#include "../ECS/CMesh.h"
#include "../ECS/CTransform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

NAMESPACES( SomeVulkan, Graphics )

Renderer::Renderer( const std::shared_ptr< InstanceContext > &context, std::shared_ptr< Scene::Camera > camera, std::shared_ptr< PipelineSelector > pipelineSelector )
        : context( context ), camera( std::move( camera ) ), pipelineSelector( std::move( pipelineSelector ) ) {
    poolSize = context->swapChainImages.size( );

    createFrameContexts( );

    meshLoader = std::make_shared< MeshLoader >( context, frameContexts[ 0 ].commandExecutor );
    textureLoader = std::make_shared< TextureLoader >( context, frameContexts[ 0 ].commandExecutor );

    createSynchronizationStructures( context->logicalDevice );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::WindowResized, [&]( const Input::EventType& event,
            const Input::pEventParameters& parameters ) {
        frameBufferResized = true;
    } );
}

void Renderer::createFrameContexts( ) {
    frameContexts.resize( poolSize, FrameContext { } );

    for ( uint32_t i = 0; i < poolSize; ++i ) {
        FrameContext &fContext = frameContexts[ i ];

        fContext.commandExecutor = std::make_shared< CommandExecutor >( context );

        fContext.transformLoader = std::make_shared< TransformLoader >( context );
        fContext.cameraLoader = std::make_shared< CameraLoader >( context, camera );

        for ( const PipelineInstance& instance: pipelineSelector->selectAll() ) {
            instance.descriptorManager->updateViewProjection( i, fContext.cameraLoader->getBuffer() );
        }
    }
}

void Renderer::drawRenderObjects( ) {
    FrameContext &currentFrameContext = frameContexts[ frameIndex ];

    currentFrameContext.cameraLoader->reload( );

    if ( currentFrameContext.cachedBuffers == nullptr ) {
        std::shared_ptr< CommandExecutor > &currentExecutor = currentFrameContext.commandExecutor;

        currentFrameContext.cachedBuffers = currentExecutor
                ->startCommandExecution( )
                ->generateBuffers( { }, context->frameBuffers.size( ) ); // TODO this could be problematic
    }

    vk::ClearColorValue colorClear = { std::array< float, 4 > { 0.0f, 0.0f, 0.0f, 1.0f } };

    currentFrameContext.cachedBuffers
            ->beginCommand( )
            ->beginRenderPass( context->frameBuffers.data( ), colorClear );

    for ( const auto &renderObject: gameEntities ) {
        refreshCommands( renderObject );
    }

    currentFrameContext.cachedBuffers
            ->endRenderPass( )
            ->execute( );
}


void Renderer::refreshCommands( const pGameEntity &entity ) {
    const auto &meshComponent = entity->getComponent< CMesh >( );
    const auto &materialComponent = entity->getComponent< CMaterial >( );
    const auto &transformComponent = entity->getComponent< CTransform >( );

    FrameContext &currentFrameContext = frameContexts[ frameIndex ];
    std::vector< vk::Framebuffer > &frameBuffers = context->frameBuffers;

    PipelineInstance& pipeline = pipelineSelector->selectPipeline( entity );
    auto& manager = pipeline.descriptorManager;

    std::vector< vk::DescriptorSet > setsToBind;

    currentFrameContext.cachedBuffers->bindRenderPass( pipeline.pipeline, vk::PipelineBindPoint::eGraphics );
    currentFrameContext.cachedBuffers->setViewport( context->viewport )->setViewScissor( context->viewScissor );

    setsToBind.emplace_back( manager->getViewProjectionDescriptorSet( frameIndex ) );


    if ( !IS_NULL( transformComponent ) ) {
        // currentFrameContext.transformLoader->load( transformComponent );

        const glm::mat4 &model = TransformLoader::getModelMatrix( transformComponent );

        currentFrameContext.cachedBuffers->pushConstant(
                pipeline.layout,
                vk::ShaderStageFlagBits::eVertex,
                4 * 4 * sizeof( float ),
                &model
        );
    }

    if ( !IS_NULL( materialComponent ) ) {
        TextureBufferList textureObject { };
        textureLoader->load( textureObject, *materialComponent );

        uint32_t i = 0;
        for ( auto &part: textureObject.texturesObjects ) {
            std::string &path = materialComponent->textures[ i ].path;

            if ( ! manager->existsSetForTexture( frameIndex, path ) ) {
                manager->updateTexture( frameIndex, path, part );
            }

            setsToBind.emplace_back( manager->getTextureDescriptorSet( frameIndex, path, i ) );
        }
    }

    FUNCTION_BREAK( IS_NULL( meshComponent ) )

    ObjectBufferList objectBuffer { };
    meshLoader->load( objectBuffer, *meshComponent );

    uint32_t offset = 0;
    for ( auto &part: objectBuffer.buffers ) {
        currentFrameContext.cachedBuffers->bindDescriptorSet( pipeline.layout, setsToBind );
        currentFrameContext.cachedBuffers->bindVertexMemory( part.vertexBuffer.first , offset );

        if ( part.indexCount > 0 ) {
            currentFrameContext.cachedBuffers->bindIndexMemory( part.indexBuffer.first, offset );
            currentFrameContext.cachedBuffers->drawIndexed( part.indexCount );
        } else {
            currentFrameContext.cachedBuffers->draw( part.vertexCount );
        }
    }
}

void Renderer::createSynchronizationStructures( const vk::Device &device ) {
    vk::SemaphoreCreateInfo semaphoreCreateInfo { };

    vk::FenceCreateInfo fenceCreateInfo { };
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    this->imageAvailableSemaphores.resize( this->poolSize );
    this->renderFinishedSemaphores.resize( this->poolSize );

    this->inFlightFences.resize( this->poolSize );
    this->imagesInFlight.resize( this->context->swapChainImages.size( ), nullptr );

    for ( uint32_t i = 0; i < this->imageAvailableSemaphores.size( ); ++i ) {
        this->imageAvailableSemaphores[ i ] = device.createSemaphore( semaphoreCreateInfo );
        this->renderFinishedSemaphores[ i ] = device.createSemaphore( semaphoreCreateInfo );
        this->inFlightFences[ i ] = device.createFence( fenceCreateInfo );
    }
}

void Renderer::render( ) {
    FrameContext &fContext = frameContexts[ frameIndex ];

    auto waitResult = context->logicalDevice.waitForFences( 1, &inFlightFences[ frameIndex ], VK_TRUE, UINT64_MAX );

    VkCheckResult( waitResult );

    uint32_t nextImage;
    auto result = context->logicalDevice.acquireNextImageKHR( context->swapChain, UINT64_MAX, imageAvailableSemaphores[ frameIndex ], nullptr );

    if ( result.result == vk::Result::eErrorOutOfDateKHR ) {
        Input::GlobalEventHandler::Instance().triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
        return;
    } else if ( result.result != vk::Result::eSuccess && result.result == vk::Result::eSuboptimalKHR ) {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }

    nextImage = result.value;

/*    if ( std::nullptr_t != imagesInFlight[ nextImage ] ) {
        waitResult = context->logicalDevice.waitForFences( 1, &imagesInFlight[ frameIndex ], true, UINT64_MAX );
        VkCheckResult( waitResult );
    }*/

    drawRenderObjects( );

//    imagesInFlight[ nextImage ] = inFlightFences[ frameIndex ];

    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo { };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[ frameIndex ];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &fContext.cachedBuffers->getBuffers( )[ nextImage ];
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
    presentInfo.pImageIndices = &nextImage;
    presentInfo.pResults = nullptr;

    auto presentResult = context->queues[ QueueType::Presentation ].presentKHR( presentInfo );

    if ( presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR || frameBufferResized ) {
        frameBufferResized = false;
        Input::GlobalEventHandler::Instance().triggerEvent( Input::EventType::SwapChainInvalidated, nullptr );
        return;
    } else if ( presentResult != vk::Result::eSuccess ) {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }

    frameIndex = ( frameIndex + 2 ) & poolSize;
}

void Renderer::freeBuffers( ) {
    for ( FrameContext &fc: frameContexts ) {
        fc.cachedBuffers.reset( );
        fc.cachedBuffers = nullptr;
    }
}

Renderer::~Renderer( ) {
    for ( uint32_t index = 0; index < imageAvailableSemaphores.size( ); ++index ) {
        context->logicalDevice.destroySemaphore( renderFinishedSemaphores[ index ], nullptr );
        context->logicalDevice.destroySemaphore( imageAvailableSemaphores[ index ], nullptr );
        context->logicalDevice.destroyFence( inFlightFences[ index ], nullptr );
    }
}

void Renderer::addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity ) {
    const std::shared_ptr< CMesh > &cmesh = gameEntity->getComponent< CMesh >( );
    if ( !IS_NULL( cmesh )) {
        meshLoader->cache( *cmesh );
    }

    const std::shared_ptr< CMaterial > &cmat = gameEntity->getComponent< CMaterial >( );

    if ( !IS_NULL( cmat )) {
        textureLoader->cache( *cmat );
    }

    gameEntities.emplace_back( gameEntity );
}

END_NAMESPACES