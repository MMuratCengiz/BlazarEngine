/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <BlazarCore/Common.h>
#include "Pass.h"
#include "GlobalResourceTable.h"
#include "../IRenderDevice.h"
#include <BlazarCore/Logger.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct PassWrapper
{
    std::vector< std::string > dependencies;
    std::vector< std::unique_ptr< IResourceLock > > executeLocks;

    std::vector< IPipeline * > pipelines;
    std::shared_ptr< IRenderPass > renderPass;
    std::vector< std::shared_ptr< IRenderTarget > > renderTargets;

    std::vector< std::string > pipelineInputsFlat;
    std::vector< std::unordered_map< std::string, bool > > pipelineInputsMap;

    bool inputsBuilt = false;
    std::vector< std::vector< std::string > > passDependentInputs;
    std::vector< std::vector< int > > loadOnceInputs;
    std::vector< std::vector< int > > perGeometryInputs;
    std::vector< std::vector< int > > perEntityInputs;
    std::vector< std::vector< int > > perFrameInputs;
    std::vector< int > perEntityInputsFlattened;

    Pass * ref;
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

    std::vector< std::unique_ptr< std::mutex > > frameLocks;
    std::vector< std::vector< int > > entitiesUpdatedThisFrame;

    bool redrawFrame = false;
    uint32_t frameIndex = 0;
public:
    explicit RenderGraph( IRenderDevice* renderDevice, AssetManager* assetManager );
    void addEntity( ECS::IGameEntity * entity ) const;
    void updateEntity( ECS::IGameEntity * entity ) const;
    void removeEntity( ECS::IGameEntity * entity ) const;

    void addPass( Pass * pass );
    void buildGraph( );

    void prepare( ECS::ComponentTable * componentTable );
    void execute( );

    const ShaderUniformBinder* getResourceBinder( ) const {  return globalResourceTable->getResourceBinder( ); }
    ~RenderGraph( );
private:
    void preparePass( PassWrapper &pass );
    void executePass( const PassWrapper &pass );
    void bindDependentInputs( const PassWrapper &pass, std::shared_ptr< IRenderPass > &renderPass, int pipelineIndex );

    void prepareInputs( PassWrapper &pass ) const;
    void drawEntity( const PassWrapper& pass, const std::shared_ptr<IRenderPass>& renderPass, const EntityWrapper& wrapper ) const;
};

END_NAMESPACES

