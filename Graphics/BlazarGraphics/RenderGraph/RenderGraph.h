#pragma once

#include <BlazarCore/Common.h>
#include "Pass.h"
#include "GlobalResourceTable.h"
#include "../IRenderDevice.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )


struct PassWrapper
{
    std::vector< std::string > dependencies;
    std::vector< std::vector< std::string > > adaptedInputs;
    std::vector< std::shared_ptr< IResourceLock > > executeLocks;

    std::vector< std::shared_ptr< IPipeline > > pipelines;
    std::shared_ptr< IRenderPass > renderPass;
    std::vector< std::shared_ptr< IRenderTarget > > renderTargets;
    std::vector< std::string > pipelineInputsFlat;
    std::unordered_map< std::string, bool > pipelineInputsMap;

    std::shared_ptr< Pass > ref;
    std::string inputGeometry;
    bool usesGeometryData = false;
};

class RenderGraph
{
private:
    IRenderDevice* renderDevice;
    AssetManager* assetManager;
    std::unique_ptr< GlobalResourceTable > globalResourceTable;

    std::vector< PassWrapper > passes;
    std::unordered_map< std::string, uint32_t > passMap;
    std::unordered_map< std::string, std::string > pipelineInputOutputDependencies;

    bool redrawFrame = false;
    uint32_t frameIndex = 0;
public:
    explicit RenderGraph( IRenderDevice* renderDevice, AssetManager* assetManager );
    void addEntity( const std::shared_ptr< ECS::IGameEntity > &entity );
    void updateEntity( const std::shared_ptr< ECS::IGameEntity > &entity );
    void removeEntity( const std::shared_ptr< ECS::IGameEntity > &entity );

    void addPass( std::shared_ptr< Pass > pass );
    void buildGraph( );

    void prepare( const std::shared_ptr< ECS::ComponentTable >& componentTable );
    void execute( );
    ~RenderGraph( );
private:
    void preparePass( PassWrapper &pass );
    void executePass( const PassWrapper &pass );
    void bindAdaptedInputs( const PassWrapper &pass, std::shared_ptr< IRenderPass > &renderPass, int pipelineIndex, const bool& bindPerFrame );
};

END_NAMESPACES

