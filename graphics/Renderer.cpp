//
// Created by Murat on 10/20/2020.
//

#include "Renderer.h"
#include "../input/GlobalEventHandler.h"
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "../external/loaders/stb_image.h"
#include "GraphicsException.h"
#include "Texture.h"

SomeVulkan::Graphics::Renderer::Renderer( const std::shared_ptr< RenderContext > &context,
                                          const std::shared_ptr< ShaderLayout > &shaderLayout )
        : context( context ), shaderLayout( shaderLayout ) {
    poolSize = context->swapChainImages.size( );

    createFrameContexts( );

    triangle = std::make_shared< RenderObject::Triangle2D >( );
    addRenderObject( ENTITY_CAST( triangle ) );

    createSynchronizationStructures( context->logicalDevice );

    auto pFunction = [ ]( void *userP, int width, int height ) -> void {
        ( ( Renderer * ) userP )->frameBufferResized = true;
    };

    SomeVulkan::Input::GlobalEventHandler::Instance( ).addWindowResizeCallback( context->window, this, pFunction );
}

void SomeVulkan::Graphics::Renderer::createFrameContexts( ) {
    frameContexts.resize( poolSize, { } );

    for ( uint32_t i = 0; i < poolSize; ++i ) {

        FrameContext &fContext = frameContexts[ i ];

        fContext.commandExecutor = std::make_shared< CommandExecutor >( context );

        fContext.ibo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        fContext.ibo.bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;


        fContext.vbo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        fContext.vbo.bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        ensureMemorySize( INITIAL_IBO_SIZE, fContext.ibo );
        ensureMemorySize( INITIAL_VBO_SIZE, fContext.vbo );

        fContext.ubo.resize( shaderLayout->getDescriptorCount( ), { } );

        for ( uint32_t index = 0; index < shaderLayout->getDescriptorCount( ); ++index ) {
            const DescriptorSetBinding &binding = shaderLayout->getDescriptorSetBindings( )[ index ];

            fContext.ubo[ index ].properties =
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            if ( binding.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
                fContext.ubo[ index ].bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                fContext.ubo[ index ].bufferType = DeviceBufferType::Regular;

                ensureMemorySize( INITIAL_UBO_SIZE, fContext.ubo[ index ] );

                BindingUpdateInfo updateInfo;
                updateInfo.index = index;
                updateInfo.parent = context->descriptorSets[ i ];
                updateInfo.memory = fContext.ubo[ index ];

                context->descriptorManager->updateUniformDescriptorSetBinding( updateInfo );
            }

            if ( binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
                fContext.ubo[ index ].bufferUsage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
                fContext.ubo[ index ].bufferType = DeviceBufferType::Image;
            }

        }
    }
}

float rotation = 90.0f;

void SomeVulkan::Graphics::Renderer::refreshCommands( const std::shared_ptr< Renderable > &renderable ) {
    const DrawDescription &drawDescription = renderable->getDrawDescription( );

    FrameContext &currentFrameContext = frameContexts[ frameIndex ];

    std::vector< VkFramebuffer > &frameBuffers = context->frameBuffers;

    if ( currentFrameContext.cachedBuffers == nullptr ) {
        currentFrameContext.cachedBuffers = currentFrameContext.commandExecutor
                ->startCommandExecution( )
                ->generateBuffers( 0, frameBuffers.size( ) );
    }

    transferData< char, Core::DynamicMemory >( drawDescription.vertexMemory, currentFrameContext.vbo );

    if ( drawDescription.indexedMode ) {
        transferData< uint32_t >( drawDescription.indices, currentFrameContext.ibo );
    }

    uint32_t i = 1;
    for ( auto &tex: drawDescription.textures ) {
        tex->loadIntoGPUMemory( context, currentFrameContext.commandExecutor );

        const DeviceBufferSize &size = DeviceBufferSize { .extent = { tex->getWidth( ), tex->getHeight( ) } };

        BindingUpdateInfo texUpdateInfo {
                .index = i++,
                .parent = context->descriptorSets[ frameIndex ],
                .memory = tex->getDeviceMemory( )
        };

        TextureBindingUpdateInfo textureBindingUpdateInfo { .updateInfo = texUpdateInfo, .texture = tex };
        context->descriptorManager->updateTextureDescriptorSetBinding( textureBindingUpdateInfo );
    }

//    rotation += Core::Time::getDeltaTime( );

    // TODO move this somewhere else
    glm::mat4 project = glm::perspective( glm::radians( 45.0f ), context->surfaceExtent.width /
                                                                 ( float ) context->surfaceExtent.height, 0.1f, 10.0f );

    project[ 1 ][ 1 ] = -1;

    auto v3 = [ ]( float f1, float f2, float f3 ) -> glm::vec3 {
        return glm::vec3( f1, f2, f3 );
    };

    const MVP mvp {
            .model = glm::rotate( glm::mat4( 1.0f ), glm::radians( rotation ), v3( 0.0f, 0.0f, 1.0f ) ),
            .view  = glm::lookAt( v3( 2.0f, 2.0f, 2.0f ), v3( 0.0f, 0.0f, 0.0f ), v3( 0.0f, 0.0f, 1.0f ) ),
            .projection = project
    };

    transferData< float, MVP >( mvp, currentFrameContext.ubo[ 0 ] );

    VkDeviceSize offset = 0;

    currentFrameContext.cachedBuffers
            ->beginCommand( )
            ->beginRenderPass( frameBuffers.data( ), { 0.0f, 0.0f, 0.0f, 1.0f } )
            ->bindRenderPass( VK_PIPELINE_BIND_POINT_GRAPHICS )
            ->bindVertexMemory( currentFrameContext.vbo.buffer.regular, offset )
            ->bindDescriptorSet( context->pipelineLayout, context->descriptorSets[ frameIndex ] )
            ->filter( drawDescription.indexedMode )
            ->bindIndexMemory( currentFrameContext.ibo.buffer.regular, offset )
            ->drawIndexed( drawDescription.indices )
            ->otherwise( )
            ->draw( 4 )
            ->endFilter( )
            ->endRenderPass( )
            ->execute( );
}

void SomeVulkan::Graphics::Renderer::ensureMemorySize( const DeviceBufferSize &requiredSize, DeviceMemory &dm ) {
    if ( dm.currentMemorySize == 0 || dm.currentMemorySize < requiredSize ) {
        if ( dm.bufferType == DeviceBufferType::Regular ) {
            vkDestroyBuffer( context->logicalDevice, dm.buffer.regular, nullptr );
        } else {
            vkDestroyImage( context->logicalDevice, dm.buffer.image, nullptr );
        }

        vkFreeMemory( context->logicalDevice, dm.memory, nullptr );

        allocateDeviceMemory( requiredSize, dm );

        if ( dm.bufferType == DeviceBufferType::Regular ) {
            vkBindBufferMemory( context->logicalDevice, dm.buffer.regular, dm.memory, 0 );
        } else {
            vkBindImageMemory( context->logicalDevice, dm.buffer.image, dm.memory, 0 );
        }

        dm.currentMemorySize = requiredSize;
    }
}

void SomeVulkan::Graphics::Renderer::allocateDeviceMemory( const DeviceBufferSize &size, DeviceMemory &dm ) {
    VkMemoryRequirements memoryRequirements { };

    if ( dm.bufferType == DeviceBufferType::Regular ) {
        VkBufferCreateInfo bufferCreateInfo { };

        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size.size;
        bufferCreateInfo.usage = dm.bufferUsage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if ( vkCreateBuffer( context->logicalDevice, &bufferCreateInfo, nullptr, &dm.buffer.regular ) != VK_SUCCESS ) {
            throw std::runtime_error( "failed to create vertex buffer!" );
        }

        vkGetBufferMemoryRequirements( context->logicalDevice, dm.buffer.regular, &memoryRequirements );
    } else {
        VkImageCreateInfo imageCreateInfo { };

        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = size.extent.width;
        imageCreateInfo.extent.height = size.extent.height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        imageCreateInfo.usage = dm.bufferUsage;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        if ( vkCreateImage( context->logicalDevice, &imageCreateInfo, nullptr, &dm.buffer.image ) != VK_SUCCESS ) {
            throw GraphicsException( GraphicsException::Source::Renderer, "Failed to create image!" );
        }

        vkGetImageMemoryRequirements( context->logicalDevice, dm.buffer.image, &memoryRequirements );
    }

    VkMemoryAllocateInfo memoryAllocateInfo { };
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = RenderUtilities::getMatchingMemoryType( context, dm.properties,
                                                                                 memoryRequirements );

    if ( vkAllocateMemory( context->logicalDevice, &memoryAllocateInfo, nullptr, &dm.memory ) != VK_SUCCESS ) {
        throw std::runtime_error( "failed to allocate buffer memory!" );
    }
}

void SomeVulkan::Graphics::Renderer::createSynchronizationStructures( const VkDevice &device ) {
    VkSemaphoreCreateInfo semaphoreCreateInfo { };
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo { };
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    this->imageAvailableSemaphores.resize( this->poolSize );
    this->renderFinishedSemaphores.resize( this->poolSize );

    this->inFlightFences.resize( this->poolSize );
    this->imagesInFlight.resize( this->context->swapChainImages.size( ), VK_NULL_HANDLE );


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
    FrameContext &fContext = frameContexts[ frameIndex ];

    for ( const auto &renderObject: renderObjects ) {

        vkWaitForFences( context->logicalDevice, 1, &inFlightFences[ frameIndex ], VK_TRUE, UINT64_MAX );

        uint32_t nextImage;
        VkResult result = vkAcquireNextImageKHR( context->logicalDevice, context->swapChain,
                                                 UINT64_MAX,
                                                 imageAvailableSemaphores[ frameIndex ],
                                                 VK_NULL_HANDLE, &nextImage );

        if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
            context->triggerEvent( EventType::SwapChainInvalidated );
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
        submitInfo.pCommandBuffers = &fContext.cachedBuffers->getBuffers( )[ nextImage ];
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
            context->triggerEvent( EventType::SwapChainInvalidated );
            return;
        } else if ( result != VK_SUCCESS ) {
            throw std::runtime_error( "failed to acquire swap chain image!" );
        }

    }

    frameIndex = ( frameIndex + 2 ) & poolSize;
}

// TODO test more
void SomeVulkan::Graphics::Renderer::ensureEnoughTexBuffers( uint32_t size ) {
    std::vector< DeviceMemory > &tbo = frameContexts[ frameIndex ].tbo;

    if ( size > tbo.size( ) ) {
        tbo.resize( size );
        DeviceMemory &memory = tbo[ size - 1 ];

        memory.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        memory.bufferUsage = VK_IMAGE_USAGE_SAMPLED_BIT;
        memory.bufferType = DeviceBufferType::Image;
    }
}

void SomeVulkan::Graphics::Renderer::freeBuffers( ) {
    for ( FrameContext &fc: frameContexts ) {
        fc.cachedBuffers.reset( );
        fc.cachedBuffers = nullptr;
    }
}

SomeVulkan::Graphics::Renderer::~Renderer( ) {
    for ( auto& renderable: renderObjects) {
        for ( auto& tex: renderable->getDrawDescription().textures ) {
            tex->unload();
        }
    }

    clearDeviceMemory( );

    for ( uint32_t index = 0; index < imageAvailableSemaphores.size( ); ++index ) {
        vkDestroySemaphore( context->logicalDevice, renderFinishedSemaphores[ index ], nullptr );
        vkDestroySemaphore( context->logicalDevice, imageAvailableSemaphores[ index ], nullptr );
        vkDestroyFence( context->logicalDevice, inFlightFences[ index ], nullptr );
    }
}

void SomeVulkan::Graphics::Renderer::addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity ) {
    std::shared_ptr< ECS::Renderable > renderable = gameEntity->getComponent< ECS::Renderable >( );

    if ( renderable != nullptr ) {
        renderObjects.emplace_back( renderable );
    }
}

void SomeVulkan::Graphics::Renderer::clearDeviceMemory( ) {
    for ( FrameContext &fc: frameContexts ) {
        for ( DeviceMemory &dm: fc.ubo ) {
            if ( dm.bufferType == DeviceBufferType::Regular ) {
                vkDestroyBuffer( this->context->logicalDevice, dm.buffer.regular, nullptr );
            } else {
                vkDestroyImage( this->context->logicalDevice, dm.buffer.image, nullptr );
            }

            vkFreeMemory( this->context->logicalDevice, dm.memory, nullptr );
        }

        vkDestroyBuffer( this->context->logicalDevice, fc.ibo.buffer.regular, nullptr );
        vkFreeMemory( this->context->logicalDevice, fc.ibo.memory, nullptr );

        vkDestroyBuffer( this->context->logicalDevice, fc.vbo.buffer.regular, nullptr );
        vkFreeMemory( this->context->logicalDevice, fc.vbo.memory, nullptr );

        for ( DeviceMemory texm: fc.tbo ) {
            vkDestroyImage( this->context->logicalDevice, texm.buffer.image, nullptr );
            vkFreeMemory( this->context->logicalDevice, texm.memory, nullptr );
        }
    }
}

