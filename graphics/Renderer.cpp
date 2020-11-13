//
// Created by Murat on 10/20/2020.
//

#include "Renderer.h"
#include "../input/GlobalEventHandler.h"
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

NAMESPACES( SomeVulkan, Graphics )

Renderer::Renderer( const std::shared_ptr< InstanceContext > &context,
                    const std::shared_ptr< ShaderLayout > &shaderLayout )
        : context( context ), shaderLayout( shaderLayout ) {
    poolSize = context->swapChainImages.size( );

    createFrameContexts( );

    for ( const auto &gameObject: model.getEntities( ) ) {
        addRenderObject( gameObject );
    }

    createSynchronizationStructures( context->logicalDevice );

    auto pFunction = [ ]( void *userP, int width, int height ) -> void {
        ( ( Renderer * ) userP )->frameBufferResized = true;
    };

    SomeVulkan::Input::GlobalEventHandler::Instance( ).addWindowResizeCallback( context->window, this, pFunction );
}

void Renderer::createFrameContexts( ) {
    frameContexts.resize( poolSize, FrameContext{ } );

    for ( uint32_t i = 0; i < poolSize; ++i ) {
        // TODO check again if this breaks
        FrameContext &fContext = frameContexts[ i ];

        fContext.commandExecutor = std::make_shared< CommandExecutor >( context );

        fContext.ibo.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        fContext.ibo.bufferUsage = vk::BufferUsageFlagBits::eIndexBuffer;


        fContext.vbo.properties = vk::MemoryPropertyFlagBits::eHostVisible;
        fContext.vbo.bufferUsage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer;

        ensureMemorySize( INITIAL_IBO_SIZE, fContext.ibo );
        ensureMemorySize( INITIAL_VBO_SIZE, fContext.vbo );

        fContext.ubo.resize( shaderLayout->getDescriptorCount( ), DeviceMemory{ } );

        for ( uint32_t index = 0; index < shaderLayout->getDescriptorCount( ); ++index ) {
            const DescriptorSetBinding &binding = shaderLayout->getDescriptorSetBindings( )[ index ];

            fContext.ubo[ index ].properties = vk::MemoryPropertyFlagBits::eHostVisible |
                                               vk::MemoryPropertyFlagBits::eHostCoherent;

            if ( binding.type == vk::DescriptorType::eUniformBuffer ) {
                fContext.ubo[ index ].bufferUsage = vk::BufferUsageFlagBits::eUniformBuffer;
                fContext.ubo[ index ].bufferType = DeviceBufferType::Regular;

                ensureMemorySize( INITIAL_UBO_SIZE, fContext.ubo[ index ] );

                BindingUpdateInfo updateInfo {
                        index,
                        context->descriptorSets[ i ],
                        fContext.ubo[ index ].buffer
                };

                context->descriptorManager->updateUniformDescriptorSetBinding( updateInfo );
            }

            if ( binding.type == vk::DescriptorType::eCombinedImageSampler ) {
                fContext.ubo[ index ].bufferUsage = vk::BufferUsageFlagBits::eUniformTexelBuffer;
                fContext.ubo[ index ].bufferType = DeviceBufferType::Image;
            }

        }
    }
}

float rotation = 90.0f;

void Renderer::drawRenderObjects( ) {
    FrameContext &currentFrameContext = frameContexts[ frameIndex ];

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

    currentFrameContext.vboOffset = 0;


    // TODO move this somewhere else
    glm::mat4 project = glm::perspective( glm::radians( 60.0f ), ( float ) context->surfaceExtent.width /
                                                                 ( float ) context->surfaceExtent.height, 0.1f, 10.0f );

    project[ 1 ][ 1 ] = -1;

    auto v3 = [ ]( float f1, float f2, float f3 ) -> glm::vec3 {
        return glm::vec3( f1, f2, f3 );
    };

    rotation += Core::Time::getDeltaTime( );

    MVP mvp;
    mvp.model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.view = glm::lookAt(v3(0.0f, 2.0f, 1.0f), v3(0.0f, 0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f));
    mvp.projection = project;
    

    transferData< float, MVP >( mvp, currentFrameContext.ubo[ 0 ], 0 );

    for ( const auto &renderObject: renderObjects ) {
        refreshCommands( renderObject );
    }

    currentFrameContext.cachedBuffers
            ->endRenderPass( )
            ->execute( );
}


void Renderer::refreshCommands( const std::shared_ptr< Renderable > &renderable ) {
    const DrawDescription &drawDescription = renderable->getDrawDescription( );

    FrameContext &currentFrameContext = frameContexts[ frameIndex ];

    std::vector< vk::Framebuffer > &frameBuffers = context->frameBuffers;

    VkDeviceSize offset = currentFrameContext.vboOffset;
    VkDeviceSize vertexOffset = drawDescription.vertexMemory.size( );
    VkDeviceSize indexOffset = drawDescription.indices.size( ) * sizeof( uint32_t );

    transferData< char, Core::DynamicMemory >(
            drawDescription.vertexMemory,
            currentFrameContext.vbo, offset );

    if ( drawDescription.indexedMode ) {
        DeviceBufferSize bufferSize( drawDescription.indices.size( ) * sizeof( uint32_t ) );

        transferData< uint32_t >( drawDescription.indices, currentFrameContext.vbo, offset + vertexOffset, bufferSize );
    }

    uint32_t i = 1;
    for (auto& tex : drawDescription.textures) {
        tex->loadIntoGPUMemory(context, currentFrameContext.commandExecutor);

        DeviceBufferSize& size = DeviceBufferSize{ vk::Extent2D( tex->getWidth( ), tex->getHeight( ) ) };

        BindingUpdateInfo texUpdateInfo{ };
        texUpdateInfo.index = i++;
        texUpdateInfo.parent = context->descriptorSets[frameIndex];
        texUpdateInfo.buffer = {};
        texUpdateInfo.buffer.image = tex->getImage();

        TextureBindingUpdateInfo textureBindingUpdateInfo{};
        textureBindingUpdateInfo.updateInfo = texUpdateInfo;
        textureBindingUpdateInfo.texture = tex;

        context->descriptorManager->updateTextureDescriptorSetBinding( textureBindingUpdateInfo );
    }

    currentFrameContext.cachedBuffers
            ->bindVertexMemory( currentFrameContext.vbo.buffer.regular, offset )
            ->bindDescriptorSet( context->pipelineLayout, context->descriptorSets[ frameIndex ] )
            ->filter( drawDescription.indexedMode )
            ->bindIndexMemory( currentFrameContext.vbo.buffer.regular, offset + vertexOffset )
            ->drawIndexed( drawDescription.indices )
            ->otherwise( )
            ->draw( drawDescription.vertexCount )
            ->endFilter( );

    currentFrameContext.vboOffset += vertexOffset + indexOffset;
}

void Renderer::ensureMemorySize( const DeviceBufferSize &requiredSize, DeviceMemory &dm ) {
    if ( dm.currentMemorySize == 0 || dm.currentMemorySize < requiredSize ) {
        if ( dm.bufferType == DeviceBufferType::Regular ) {
            context->logicalDevice.destroyBuffer( dm.buffer.regular );
        } else {
            context->logicalDevice.destroyImage( dm.buffer.image );
        }

        context->logicalDevice.freeMemory( dm.memory );

        allocateDeviceMemory( requiredSize, dm );

        if ( dm.bufferType == DeviceBufferType::Regular ) {
            context->logicalDevice.bindBufferMemory( dm.buffer.regular, dm.memory, 0 );
        } else {
            context->logicalDevice.bindImageMemory( dm.buffer.image, dm.memory, 0 );
        }

        dm.currentMemorySize = requiredSize;
    }
}

void Renderer::allocateDeviceMemory( const DeviceBufferSize &size, DeviceMemory &dm ) {
    VkMemoryRequirements memoryRequirements { };

    if ( dm.bufferType == DeviceBufferType::Regular ) {
        vk::BufferCreateInfo bufferCreateInfo { };

        bufferCreateInfo.size = size.size;
        bufferCreateInfo.usage = dm.bufferUsage;
        bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        dm.buffer.regular = context->logicalDevice.createBuffer( bufferCreateInfo );
        memoryRequirements = context->logicalDevice.getBufferMemoryRequirements( dm.buffer.regular );
    } else {
        vk::ImageCreateInfo imageCreateInfo { };

        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.extent.width = size.extent.width;
        imageCreateInfo.extent.height = size.extent.height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
        imageCreateInfo.tiling = vk::ImageTiling::eLinear;
        imageCreateInfo.initialLayout = vk::ImageLayout::ePreinitialized;
//        imageCreateInfo.usage = vk::ImageUsageFlags( dm.bufferUsage ); // todo care, should not be active anyway
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
        imageCreateInfo.samples = RenderUtilities::maxDeviceMSAASampleCount( context->physicalDevice );

        dm.buffer.image = context->logicalDevice.createImage( imageCreateInfo );

        memoryRequirements = context->logicalDevice.getImageMemoryRequirements( dm.buffer.image );
    }

    vk::MemoryAllocateInfo memoryAllocateInfo { };

    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = RenderUtilities::getMatchingMemoryType( context, dm.properties,
                                                                                 memoryRequirements );
    dm.memory = context->logicalDevice.allocateMemory( memoryAllocateInfo );
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
    auto result = context->logicalDevice.acquireNextImageKHR( context->swapChain, UINT64_MAX,
                                                              imageAvailableSemaphores[ frameIndex ],
                                                              nullptr );

    if ( result.result == vk::Result::eErrorOutOfDateKHR ) {
        context->triggerEvent( EventType::SwapChainInvalidated );
        return;
    } else if ( result.result != vk::Result::eSuccess && result.result == vk::Result::eSuboptimalKHR ) {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }

    nextImage = result.value;

    if ( nullptr != imagesInFlight[ nextImage ] ) {
        waitResult = context->logicalDevice.waitForFences( 1, &imagesInFlight[ frameIndex ], true, UINT64_MAX );
        VkCheckResult( waitResult );
    }

    drawRenderObjects( );

    imagesInFlight[ nextImage ] = inFlightFences[ frameIndex ];

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

    if ( presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR
         || frameBufferResized ) {
        frameBufferResized = false;
        context->triggerEvent( EventType::SwapChainInvalidated );
        return;
    } else if ( presentResult != vk::Result::eSuccess ) {
        throw std::runtime_error( "failed to acquire swap chain image!" );
    }

    frameIndex = ( frameIndex + 2 ) & poolSize;
}

// TODO test more
void Renderer::ensureEnoughTexBuffers( uint32_t size ) {
    std::vector< DeviceMemory > &tbo = frameContexts[ frameIndex ].tbo;

    if ( size > tbo.size( ) ) {
        tbo.resize( size );
        DeviceMemory &memory = tbo[ size - 1 ];

        memory.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        memory.bufferUsage = vk::BufferUsageFlagBits( vk::ImageUsageFlagBits::eSampled ); // todo problem?
        memory.bufferType = DeviceBufferType::Image;
    }
}

void Renderer::freeBuffers( ) {
    for ( FrameContext &fc: frameContexts ) {
        fc.cachedBuffers.reset( );
        fc.cachedBuffers = nullptr;
    }
}

Renderer::~Renderer( ) {
    for ( auto &renderable: renderObjects ) {
        for ( auto &tex: renderable->getDrawDescription( ).textures ) {
            tex->unload( );
        }
    }

    clearDeviceMemory( );

    for ( uint32_t index = 0; index < imageAvailableSemaphores.size( ); ++index ) {
        context->logicalDevice.destroySemaphore( renderFinishedSemaphores[ index ], nullptr );
        context->logicalDevice.destroySemaphore( imageAvailableSemaphores[ index ], nullptr );
        context->logicalDevice.destroyFence( inFlightFences[ index ], nullptr );
    }
}

void Renderer::addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity ) {
    std::shared_ptr< ECS::Renderable > renderable = gameEntity->getComponent< ECS::Renderable >( );

    if ( renderable != nullptr ) {
        renderObjects.emplace_back( renderable );
    }
}

void Renderer::clearDeviceMemory( ) {
    for ( FrameContext &fc: frameContexts ) {
        for ( DeviceMemory &dm: fc.ubo ) {
            if ( dm.bufferType == DeviceBufferType::Regular ) {
                this->context->logicalDevice.destroyBuffer( dm.buffer.regular, nullptr );
            } else {
                this->context->logicalDevice.destroyImage( dm.buffer.image, nullptr );
            }

            this->context->logicalDevice.freeMemory( dm.memory, nullptr );
        }

        this->context->logicalDevice.destroyBuffer( fc.ibo.buffer.regular, nullptr );
        this->context->logicalDevice.freeMemory( fc.ibo.memory, nullptr );

        this->context->logicalDevice.destroyBuffer( fc.vbo.buffer.regular, nullptr );
        this->context->logicalDevice.freeMemory( fc.vbo.memory, nullptr );

        for ( DeviceMemory &texm: fc.tbo ) {
            this->context->logicalDevice.destroyImage( texm.buffer.image, nullptr );
            this->context->logicalDevice.freeMemory( texm.memory, nullptr );
        }
    }
}

END_NAMESPACES