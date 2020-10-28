#pragma once

#include "../core/Common.h"
#include "CommandExecutor.h"
#include "../ECS.h"
#include "../graphics/RenderUtilities.h"

NAMESPACES( SomeVulkan, Graphics )

class CommandExecutor;
class CommandList;

using namespace ECS;

class Renderer {
private:
    const uint32_t MAX_CONCURRENT_FRAMES = 2;
    uint32_t frameIndex = 0;

    std::shared_ptr< RenderContext > context;

    std::shared_ptr< CommandExecutor > commandExecutor;
    std::shared_ptr< CommandList > cachedBuffers;

    VkBuffer vertexBuffer { };
    VkDeviceMemory vertexMemory { };
    VkBuffer stagingBuffer { };
    VkDeviceMemory stagingMemory { };

    VkBuffer indexBuffer { };
    VkDeviceMemory indexMemory { };
    std::vector< VkCommandBuffer > buffers;
    void *vMemLocation { };

    std::vector< VkSemaphore > imageAvailableSemaphores;
    std::vector< VkSemaphore > renderFinishedSemaphores;
    std::vector< VkFence > imagesInFlight;
    std::vector< VkFence > inFlightFences;
    std::vector< std::shared_ptr< Renderable > > renderObjects;

    bool frameBufferResized = false;
    VkDeviceSize currentVbBufferSize = 0;
    VkDeviceSize currentIndexBufferSize = 0;
public:
    explicit Renderer( const std::shared_ptr< RenderContext > &context );
    void addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity );
    void render( );
    void freeBuffers( );
    ~Renderer( );

    Renderer( const Renderer & ) = delete;
    Renderer &operator=( const Renderer & ) = delete;
private:

    void refreshCommands( const std::shared_ptr< Renderable > &renderable );

    void reallocateDeviceMemory( std::shared_ptr< Renderable > renderObject );

    void allocateDeviceMemory(
            VkDeviceSize size,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkBuffer &buffer,
            VkDeviceMemory &deviceMemory );

    void transferData(
            const std::shared_ptr< Renderable > &renderObject,
            void *data, VkBuffer &targetBuffer,
            VkDeviceMemory &targetMemory,
            VkBufferUsageFlags usage );

    void createSynchronizationStructures( const VkDevice &device );
    void clearDeviceMemory( );
};

END_NAMESPACES