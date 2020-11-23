//
// Created by Murat on 10/20/2020.
//

#include "Renderer.h"
#include "../Input/GlobalEventHandler.h"
#include "../ECS/CMesh.h"
#include "../ECS/CTransform.h"
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

NAMESPACES( SomeVulkan, Graphics )

Renderer::Renderer( const std::shared_ptr< InstanceContext > &context, std::shared_ptr< Scene::Camera > camera, const std::shared_ptr< GLSLShaderSet > &shaderSet )
        : context( context ), camera( std::move( camera ) ), shaderSet( shaderSet ) {
    poolSize = context->swapChainImages.size( );

    if ( descriptorManager == nullptr ) {
        descriptorManager = std::make_shared< DescriptorManager >( context, shaderSet );
    }

    createFrameContexts( );

    meshLoader = std::make_shared< SMeshLoader >( context, frameContexts[ 0 ].commandExecutor );
    textureLoader = std::make_shared< STextureLoader >( context, frameContexts[ 0 ].commandExecutor );

    createSynchronizationStructures( context->logicalDevice );

    auto pFunction = [ ]( void *userP, int width, int height ) -> void {
        ( ( Renderer * ) userP )->frameBufferResized = true;
    };

    SomeVulkan::Input::GlobalEventHandler::Instance( ).addWindowResizeCallback( context->window, this, pFunction );
}

void Renderer::createFrameContexts( ) {
    frameContexts.resize( poolSize, FrameContext { } );

    for ( uint32_t i = 0; i < poolSize; ++i ) {
        FrameContext &fContext = frameContexts[ i ];

        fContext.commandExecutor = std::make_shared< CommandExecutor >( context );

        fContext.transformLoader = std::make_shared< STransformLoader >( context );
        fContext.cameraLoader = std::make_shared< SCameraLoader >( context, camera );

        BindingUpdateInfo updateInfo {
                0,
                context->descriptorSets[ i ],
                fContext.cameraLoader->getBuffer()
        };

        descriptorManager->updateUniformDescriptorSetBinding( updateInfo );

        updateInfo.index = 1;
        updateInfo.buffer = fContext.transformLoader->getBuffer();

        descriptorManager->updateUniformDescriptorSetBinding( updateInfo );
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
            ->beginRenderPass( context->frameBuffers.data( ), colorClear )
            ->bindRenderPass( vk::PipelineBindPoint::eGraphics );

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

    if ( !IS_NULL( transformComponent ) ) {
        currentFrameContext.transformLoader->load( transformComponent );
    }

    if ( !IS_NULL( materialComponent ) ) {
        TextureBufferList textureObject { };
        textureLoader->load( textureObject, *materialComponent );

        uint32_t i = 2;
        for ( auto &part: textureObject.texturesObjects ) {
            BindingUpdateInfo texUpdateInfo { };
            texUpdateInfo.index = i++;
            texUpdateInfo.parent = context->descriptorSets[ frameIndex ];

            TextureBindingUpdateInfo textureBindingUpdateInfo { };
            textureBindingUpdateInfo.updateInfo = texUpdateInfo;
            textureBindingUpdateInfo.texture = part;

            descriptorManager->updateTextureDescriptorSetBinding( textureBindingUpdateInfo );
            currentFrameContext.cachedBuffers->bindDescriptorSet( context->pipelineLayout, context->descriptorSets[ frameIndex ] );
        }
    }

    FUNCTION_BREAK( IS_NULL( meshComponent ) )

    ObjectBufferList objectBuffer { };

    std::vector< uint64_t > offsets;
    std::vector< vk::Buffer > vbuffers;
    std::vector< vk::Buffer > ibuffers;

    uint64_t totalVertexCount = 0;
    uint64_t totalIndexCount = 0;

    meshLoader->load( objectBuffer, *meshComponent );

    for ( auto &part: objectBuffer.buffers ) {
        vbuffers.emplace_back( part.vertexBuffer.first );
        offsets.emplace_back( 0 ); // ?

        if ( part.indexCount > 0 ) {
            ibuffers.emplace_back( part.indexBuffer.first );
            totalIndexCount += part.indexCount;
        }

        totalVertexCount += part.vertexCount;
    }

    currentFrameContext.cachedBuffers->bindVertexMemory( vbuffers[ 0 ], offsets[ 0 ] );

    if ( !ibuffers.empty( ) ) {
        currentFrameContext.cachedBuffers->bindIndexMemory( ibuffers[ 0 ], offsets[ 0 ] );
        currentFrameContext.cachedBuffers->drawIndexed( totalIndexCount );
    } else {
        currentFrameContext.cachedBuffers->draw( totalVertexCount );
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
        context->triggerEvent( EventType::SwapChainInvalidated );
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
        context->triggerEvent( EventType::SwapChainInvalidated );
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