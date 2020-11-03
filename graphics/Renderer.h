#pragma once

#include "../core/Common.h"
#include "CommandExecutor.h"
#include "../ECS.h"
#include "RenderUtilities.h"
#include "RendererTypes.h"
#include "DefaultShaderLayout.h"
#include "../renderobjects/Triangle2D.h"

NAMESPACES( SomeVulkan, Graphics )

using namespace ECS;

class Renderer {
private:
    const DeviceBufferSize INITIAL_VBO_SIZE = DeviceBufferSize { .size = 100 * sizeof( float ) };
    const DeviceBufferSize INITIAL_IBO_SIZE = DeviceBufferSize { .size = 100 * sizeof( uint32_t ) };
    const DeviceBufferSize INITIAL_UBO_SIZE = DeviceBufferSize { .size = 3 * 4 * 4 * sizeof( float ) };

    const DeviceBufferSize INITIAL_TEX_SIZE = DeviceBufferSize { .extent = { 100, 100 } };

    uint32_t poolSize = 3;
    uint32_t frameIndex = 0;

    std::shared_ptr< RenderContext > context;
    std::vector< FrameContext > frameContexts;
    std::vector< VkCommandBuffer > buffers;

    std::vector< VkSemaphore > imageAvailableSemaphores;
    std::vector< VkSemaphore > renderFinishedSemaphores;
    std::vector< VkFence > imagesInFlight;
    std::vector< VkFence > inFlightFences;
    std::vector< std::shared_ptr< Renderable > > renderObjects;

    bool frameBufferResized = false;
    VkDeviceSize currentVbBufferSize = 0;
    VkDeviceSize currentIndexBufferSize = 0;
    std::shared_ptr< ShaderLayout > shaderLayout;
    std::shared_ptr< RenderObject::Triangle2D > triangle;
public:
    explicit Renderer( const std::shared_ptr< RenderContext > &context, const std::shared_ptr< ShaderLayout >& shaderLayout );
    void addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity );
    void render( );
    void freeBuffers( );
    ~Renderer( );

    Renderer( const Renderer & ) = delete;
    Renderer &operator=( const Renderer & ) = delete;

    inline FrameContext& getFrameContext( uint32_t image ) {
        return frameContexts[ image ];
    }
private:

    template< class T, class V = std::vector< T > >
    void transferData( const V &v, DeviceMemory &targetMemory) {
        transferData< T, V >( v, targetMemory, DeviceBufferSize { .size = v.size() });
    }

    template< class T, class V = std::vector< T > >
    void transferData( const V &v, DeviceMemory &targetMemory, const DeviceBufferSize& bufferSize ) {
        ensureMemorySize( bufferSize, targetMemory );

        RenderUtilities::copyToDeviceMemory(
                context->logicalDevice,
                targetMemory.memory,
                ( const void * ) v.data( ),
                v.size( ) * sizeof( T )
        );
    }

    void refreshCommands( const std::shared_ptr< Renderable > &renderable );
    void ensureMemorySize( const DeviceBufferSize &requiredSize, DeviceMemory &memory );
    void allocateDeviceMemory( const DeviceBufferSize &size, DeviceMemory &dm );
    void createSynchronizationStructures( const VkDevice &device );
    void clearDeviceMemory( );
    void createFrameContexts( );
    void ensureEnoughTexBuffers( uint32_t size );
};

END_NAMESPACES