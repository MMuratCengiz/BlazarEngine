#pragma once

#include "CommandExecutor.h"
#include <BlazarECS/ECS.h>
#include "RenderUtilities.h"
#include "MeshLoader.h"
#include "GLSLShaderSet.h"
#include "MaterialLoader.h"
#include "CameraLoader.h"
#include "TransformLoader.h"
#include "DescriptorManager.h"
#include "PipelineSelector.h"
#include "CubeMapLoader.h"
#include "LightLoader.h"
#include "WorldContextLoader.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

using namespace ECS;

struct FrameContext
{
    std::shared_ptr< CommandExecutor > commandExecutor;
    std::shared_ptr< CommandList > cachedBuffers;
    std::shared_ptr< TransformLoader > transformLoader;
    std::shared_ptr< CameraLoader > cameraLoader;
    std::shared_ptr< LightLoader > lightLoader;
    std::shared_ptr< WorldContextLoader > worldContextLoader;
    uint32_t currentActiveImage;
};

class Renderer : public ISystem
{
private:
    uint32_t poolSize = 3;
    uint32_t frameIndex = 0;

    std::shared_ptr< InstanceContext > context;

    std::vector< FrameContext > frameContexts;
    std::vector< vk::CommandBuffer > buffers;

    std::vector< vk::Semaphore > imageAvailableSemaphores;
    std::vector< vk::Semaphore > renderFinishedSemaphores;
    std::vector< vk::Fence > imagesInFlight;
    std::vector< vk::Fence > inFlightFences;

    bool frameBufferResized = false;
    bool sceneInitialized = false;

    std::shared_ptr< GLSLShaderSet > shaderSet;
    std::shared_ptr< ECS::CCamera > camera;
    std::shared_ptr< PipelineSelector > pipelineSelector;

    std::shared_ptr< MeshLoader > meshLoader;
    std::shared_ptr< MaterialLoader > materialLoader;
    std::shared_ptr< CubeMapLoader > cubeMapLoader;
public:
    explicit Renderer( const std::shared_ptr< InstanceContext > &context, std::shared_ptr< PipelineSelector > pipelineSelector );

    // ISystem overrides
    void frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;
    void entityTick( const std::shared_ptr< IGameEntity > &gameEntity ) override;
    void frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;
    // --ISystem overrides

    void freeBuffers( );
    ~Renderer( );

    Renderer( const Renderer & ) = delete;
    Renderer &operator=( const Renderer & ) = delete;

    inline FrameContext &getFrameContext( uint32_t image )
    {
        return frameContexts[ image ];
    }

private:
    void initScene( const std::shared_ptr< ECS::ComponentTable >& componentTable );
    void createSynchronizationStructures( const vk::Device &device );
    void createFrameContexts( );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  Loads Custom Components //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void updateCubeComponent( const std::shared_ptr< IGameEntity > &entity, std::shared_ptr< DescriptorManager > &manager, std::vector< vk::DescriptorSet > &setsToBind );
    void updateMaterialComponent( const std::shared_ptr< IGameEntity > &entity, std::shared_ptr< DescriptorManager > &manager, std::vector< vk::DescriptorSet > &setsToBind );
    void updateTransformComponent( const pGameEntity &entity, const FrameContext &currentFrameContext, const PipelineInstance &pipeline ) const;
};

END_NAMESPACES