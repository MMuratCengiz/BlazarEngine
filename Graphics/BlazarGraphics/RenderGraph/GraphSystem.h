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
#include <BlazarECS/ECS.h>
#include <BlazarInput/GlobalEventHandler.h>
#include "RenderGraph.h"
#include "CommonPasses.h"
#include "../IRenderDevice.h"
#include "../AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class GraphSystem : public ECS::ISystem
{
private:
    std::unique_ptr< RenderGraph > renderGraph;
    IRenderDevice* renderDevice;
    AssetManager* assetManager;
    bool isSystemActive = true;

    // temporary remove later:
    std::unique_ptr< Pass > shadowMapPass;
    std::unique_ptr< Pass > gBufferPass;
    std::unique_ptr< Pass > lightingPass;
    std::unique_ptr< Pass > skyBoxPass;
    std::unique_ptr< Pass > presentPass;
public:
    GraphSystem( IRenderDevice* renderDevice, AssetManager* assetManager );
    void addEntity( ECS::IGameEntity* entity ) override;
    void updateEntity( ECS::IGameEntity* entity ) override;
    void removeEntity( ECS::IGameEntity* entity ) override;

    void frameStart( ECS::ComponentTable * componentTable ) override;
    void entityTick( ECS::IGameEntity* entity ) override;
    void frameEnd( ECS::ComponentTable * componentTable ) override;
    void cleanup( ) override;
};

END_NAMESPACES