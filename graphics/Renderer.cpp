//
// Created by Murat on 10/20/2020.
//

#include "Renderer.h"

#include <utility>
#include "../input/GlobalEventHandler.h"
#include "../renderobjects/Triangle2D.h"

SomeVulkan::Graphics::Renderer::Renderer( const std::shared_ptr< RenderContext >&  context ) : context( context ) {
    commandExecutor = std::make_shared< CommandExecutor >( context );

    auto triangle = std::make_shared< SomeVulkan::RenderObject::Triangle2D >( );

    addRenderObject( ENTITY_CAST( triangle ) );

    createSynchronizationStructures( context->logicalDevice );

    void (*pFunction)( void *, int, int ) = [ ]( void *userP, int width,
                                                 int height ) {
        ( ( Renderer * ) userP )->frameBufferResized = true;
    };

    SomeVulkan::Input::GlobalEventHandler::Instance( ).addWindowResizeCallback( context->window, this,
                                                                                pFunction );
}

void SomeVulkan::Graphics::Renderer::refreshCommands( const std::shared_ptr< Renderable > &renderable ) {
    const VertexDescriptor &vertexDescriptor = renderable->getVertexDescriptor( );

    auto vertices = vertexDescriptor.vertices;

    transferData( renderable, vertices.data( ), vertexBuffer, vertexMemory,
                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );

    if ( vertexDescriptor.indexedMode ) {
        transferData( renderable, ( void * ) vertexDescriptor.indices.data( ), indexBuffer, indexMemory,
                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT );
    }

    VkDeviceSize offset = 0;

    std::vector< VkFramebuffer > &frameBuffers = context->frameBuffers;

    if ( cachedBuffers == nullptr ) {
        cachedBuffers = commandExecutor
                ->startCommandExecution( )
                ->generateBuffers( 0, frameBuffers.size( ) );
    }

    cachedBuffers
            ->beginCommand( )
            ->beginRenderPass( frameBuffers.data( ), { 0.0f, 0.0f, 0.0f, 1.0f } )
            ->bindRenderPass( VK_PIPELINE_BIND_POINT_GRAPHICS )
            ->bindVertexMemory( vertexBuffer, offset )
            ->filter( vertexDescriptor.indexedMode )
            ->bindIndexMemory( indexBuffer, offset )
            ->drawIndexed( vertexDescriptor.indices )
            ->otherwise( )
            ->draw( 4 )
            ->endFilter( )
            ->endRenderPass( )
            ->execute( );
}

void SomeVulkan::Graphics::Renderer::reallocateDeviceMemory( std::shared_ptr< Renderable > renderObject ) {
    VkDeviceSize size = renderObject->getVertexDescriptor().vertices.size() * sizeof( float );
    VkDeviceSize indexSize = renderObject->getVertexDescriptor().indices.size() * sizeof( uint32_t );

    if ( size > currentVbBufferSize ) {
        if ( currentVbBufferSize != 0 ) {
            clearDeviceMemory();
        }

        allocateDeviceMemory( size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer,
                              stagingMemory );

        allocateDeviceMemory( size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vertexBuffer,
                vertexMemory );


        currentVbBufferSize = size;
        vkBindBufferMemory( context->logicalDevice, stagingBuffer, stagingMemory, 0 );
        vkBindBufferMemory( context->logicalDevice, vertexBuffer, vertexMemory, 0 );
    }

    if ( indexSize > currentIndexBufferSize ) {
        if ( currentIndexBufferSize != 0 ) {
            vkDestroyBuffer( context->logicalDevice, indexBuffer, nullptr );
            vkFreeMemory( context->logicalDevice, indexMemory, nullptr );
        }

        allocateDeviceMemory( size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              indexBuffer,
                              indexMemory );

        currentIndexBufferSize = indexSize;

        vkBindBufferMemory( context->logicalDevice, indexBuffer, indexMemory, 0 );
    }

}

void SomeVulkan::Graphics::Renderer::allocateDeviceMemory( VkDeviceSize size, VkBufferUsageFlags usageFlags,
                                                               VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer &buffer,
                                                               VkDeviceMemory &deviceMemory ) {
    VkBufferCreateInfo bufferCreateInfo { };

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usageFlags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if ( vkCreateBuffer( context->logicalDevice, &bufferCreateInfo, nullptr, &buffer ) != VK_SUCCESS ) {
        throw std::runtime_error( "failed to create vertex buffer!" );
    }

    VkMemoryRequirements memoryRequirements { };
    vkGetBufferMemoryRequirements( context->logicalDevice, buffer, &memoryRequirements );

    VkPhysicalDeviceMemoryProperties memoryProperties { };
    vkGetPhysicalDeviceMemoryProperties( context->physicalDevice, &memoryProperties );

    uint32_t index = 0;
    for ( ; index < memoryProperties.memoryTypeCount; ++index ) {
        VkMemoryType &type = memoryProperties.memoryTypes[ index ];
        if ( ( type.propertyFlags & memoryPropertyFlags ) != 0 ) {
            break;
        }
    }

    VkMemoryAllocateInfo memoryAllocateInfo { };
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = index;

    if ( vkAllocateMemory( context->logicalDevice, &memoryAllocateInfo, nullptr, &deviceMemory ) != VK_SUCCESS ) {
        throw std::runtime_error( "failed to allocate vertex buffer memory!" );
    }
}

void SomeVulkan::Graphics::Renderer::transferData(
        const std::shared_ptr< Renderable >& renderObject,
        void *data,
        VkBuffer &targetBuffer,
        VkDeviceMemory &targetMemory,
        VkBufferUsageFlags usage ) {
    VkDeviceSize bufferSize = renderObject->getVertexDescriptor().vertices.size() * sizeof( float );

    reallocateDeviceMemory( renderObject );

    RenderUtilities::copyToDeviceMemory(
            context->logicalDevice,
            stagingMemory,
            vMemLocation,
            data,
            bufferSize
    );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT )
            ->beginCommand( )
            ->copyBuffer( bufferSize, stagingBuffer, targetBuffer )
            ->execute( );
}

void SomeVulkan::Graphics::Renderer::createSynchronizationStructures( const VkDevice &device ) {
    VkSemaphoreCreateInfo semaphoreCreateInfo { };
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo { };
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    this->imageAvailableSemaphores.resize( this->MAX_CONCURRENT_FRAMES );
    this->renderFinishedSemaphores.resize( this->MAX_CONCURRENT_FRAMES );

    this->inFlightFences.resize( this->MAX_CONCURRENT_FRAMES );
    this->imagesInFlight.resize( this->context->images.size( ), VK_NULL_HANDLE );


    for ( uint32_t i = 0; i < this->imageAvailableSemaphores.size( ); ++i ) {
        VkResult r1 = vkCreateSemaphore( device, &semaphoreCreateInfo, nullptr, &this->imageAvailableSemaphores[ i ] );
        VkResult r2 = vkCreateSemaphore( device, &semaphoreCreateInfo, nullptr, &this->renderFinishedSemaphores[ i ] );
        VkResult r3 = vkCreateFence( device, &fenceCreateInfo, nullptr, &this->inFlightFences[ i ] );

        if ( r1 != VK_SUCCESS || r2 != VK_SUCCESS || r3 != VK_SUCCESS ) {
            throw std::runtime_error( "failed to create semaphores!" );
        }
    }
}

void SomeVulkan::Graphics::Renderer::render( ) {
    for ( const auto &renderObject: renderObjects ) {

        vkWaitForFences( context->logicalDevice, 1, &inFlightFences[ frameIndex ], VK_TRUE, UINT64_MAX );

        uint32_t nextImage;
        VkResult result = vkAcquireNextImageKHR( context->logicalDevice, context->swapChain,
                                                 UINT64_MAX,
                                                 imageAvailableSemaphores[ frameIndex ],
                                                 VK_NULL_HANDLE, &nextImage );

        if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
            cachedBuffers.reset( );
            return;
        } else if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ) {
            throw std::runtime_error( "failed to acquire swap chain image!" );
        }

        if ( imagesInFlight[ nextImage ] != VK_NULL_HANDLE ) {
            vkWaitForFences( context->logicalDevice, 1, &imagesInFlight[ frameIndex ], VK_TRUE, UINT64_MAX );
        }

        refreshCommands( renderObject );


        imagesInFlight[ nextImage ] = inFlightFences[ frameIndex ];

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores[ frameIndex ];
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cachedBuffers->getBuffers( )[ nextImage ];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[ frameIndex ];

        vkResetFences( context->logicalDevice, 1, &inFlightFences[ frameIndex ] );

        if ( vkQueueSubmit( context->queues[ QueueType::Graphics ], 1, &submitInfo,
                            inFlightFences[ frameIndex ] ) ) {
            throw std::runtime_error( "failed to submit draw command buffer!" );
        }

        VkPresentInfoKHR presentInfo { };
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[ frameIndex ];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &context->swapChain;
        presentInfo.pImageIndices = &nextImage;
        presentInfo.pResults = nullptr;

        result = vkQueuePresentKHR( context->queues[ QueueType::Presentation ], &presentInfo );

        if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized ) {
            frameBufferResized = false;
            cachedBuffers.reset( );
            return;
        } else if ( result != VK_SUCCESS ) {
            throw std::runtime_error( "failed to acquire swap chain image!" );
        }

    }

    frameIndex = ( frameIndex + 1 ) & MAX_CONCURRENT_FRAMES;
}

void SomeVulkan::Graphics::Renderer::freeBuffers( ) {
    cachedBuffers->freeBuffers( );
}

SomeVulkan::Graphics::Renderer::~Renderer( ) {
    clearDeviceMemory( );

    vkDestroyBuffer( context->logicalDevice, indexBuffer, nullptr );
    vkFreeMemory( context->logicalDevice, indexMemory, nullptr );

    for ( uint32_t index = 0; index < imageAvailableSemaphores.size( ); ++index ) {
        vkDestroySemaphore( context->logicalDevice, renderFinishedSemaphores[ index ], nullptr );
        vkDestroySemaphore( context->logicalDevice, imageAvailableSemaphores[ index ], nullptr );
        vkDestroyFence( context->logicalDevice, inFlightFences[ index ], nullptr );
    }
}

void SomeVulkan::Graphics::Renderer::clearDeviceMemory( ) {
    vkDestroyBuffer( this->context->logicalDevice, this->stagingBuffer, nullptr );
    vkFreeMemory( this->context->logicalDevice, this->stagingMemory, nullptr );

    vkDestroyBuffer( this->context->logicalDevice, this->vertexBuffer, nullptr );
    vkFreeMemory( this->context->logicalDevice, this->vertexMemory, nullptr );
}

void SomeVulkan::Graphics::Renderer::addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity ) {
    std::shared_ptr< ECS::Renderable > renderable = gameEntity->getComponent< ECS::Renderable >( );

    if ( renderable != nullptr ) {
        renderObjects.emplace_back( renderable );
    }
}

