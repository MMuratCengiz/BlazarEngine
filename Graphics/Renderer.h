#pragma once

#include "../Core/Common.h"
#include "CommandExecutor.h"
#include "../ECS.h"
#include "RenderUtilities.h"
#include "SMeshLoader.h"
#include "GLSLShaderSet.h"
#include "STextureLoader.h"
#include "SCameraLoader.h"
#include "STransformLoader.h"
#include "DescriptorManager.h"

NAMESPACES( SomeVulkan, Graphics )

using namespace ECS;

struct FrameContext {
    std::shared_ptr< CommandExecutor > commandExecutor;
    std::shared_ptr< CommandList > cachedBuffers;
    std::shared_ptr< STransformLoader > transformLoader;
    std::shared_ptr< SCameraLoader > cameraLoader;
};

class Renderer {
private:
    uint32_t poolSize = 3;
    uint32_t frameIndex = 0;

    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< DescriptorManager > descriptorManager;

    std::vector< FrameContext > frameContexts;
    std::vector< vk::CommandBuffer > buffers;

    std::vector< vk::Semaphore > imageAvailableSemaphores;
    std::vector< vk::Semaphore > renderFinishedSemaphores;
    std::vector< vk::Fence > imagesInFlight;
    std::vector< vk::Fence > inFlightFences;
    std::vector< pGameEntity > gameEntities;

    bool frameBufferResized = false;
    vk::DeviceSize currentVbBufferSize = 0;
    vk::DeviceSize currentIndexBufferSize = 0;
    std::shared_ptr< GLSLShaderSet > shaderSet;
    std::shared_ptr< Scene::Camera > camera;

    std::shared_ptr< SMeshLoader > meshLoader;
    std::shared_ptr< STextureLoader > textureLoader;
public:
    explicit Renderer( const std::shared_ptr< InstanceContext > &context, std::shared_ptr< Scene::Camera > camera, const std::shared_ptr< GLSLShaderSet > &shaderSet );
    void addRenderObject( const std::shared_ptr< IGameEntity > &gameEntity );
    void render( );
    void freeBuffers( );
    ~Renderer( );

    Renderer( const Renderer & ) = delete;
    Renderer &operator=( const Renderer & ) = delete;

    inline FrameContext &getFrameContext( uint32_t image ) {
        return frameContexts[ image ];
    }
private:
    void drawRenderObjects( );
    void refreshCommands( const pGameEntity &entity );
    void createSynchronizationStructures( const vk::Device &device );
    void createFrameContexts( );
};

END_NAMESPACES