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

#include "GraphSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

GraphSystem::GraphSystem( IRenderDevice *renderDevice, AssetManager *assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
    renderGraph = std::make_unique< RenderGraph >( this->renderDevice, this->assetManager );

    shadowMapPass = CommonPasses::createShadowMapPass( this->renderDevice );
    gBufferPass = CommonPasses::createGBufferPass( this->renderDevice );
    lightingPass = CommonPasses::createLightingPass( this->renderDevice );
    skyBoxPass = CommonPasses::createSkyBoxPass( this->renderDevice );
    presentPass = CommonPasses::createPresentPass( this->renderDevice );

    renderGraph->addPass( shadowMapPass.get( ) );
    renderGraph->addPass( gBufferPass.get( ) );
    renderGraph->addPass( lightingPass.get( ) );
    renderGraph->addPass( skyBoxPass.get( ) );
    //renderGraph->addPass( CommonPasses::createSMAAEdgePass( this->renderDevice ) );
    //renderGraph->addPass( CommonPasses::createSMAABlendWeightPass( this->renderDevice ) );
    //renderGraph->addPass( CommonPasses::createSMAANeighborPass( this->renderDevice ) );
    renderGraph->addPass( presentPass.get( ) );

    renderGraph->buildGraph( );

    Input::Events::subscribe< Input::WindowResizedParameters * >( Input::EventType::WindowResized, [ & ]( Input::WindowResizedParameters * parameters )
    {
        isSystemActive = parameters->width > 0 && parameters->height > 0;
    } );
}

void GraphSystem::addEntity( ECS::IGameEntity * entity )
{
    renderGraph->addEntity( entity );
}

void GraphSystem::updateEntity( ECS::IGameEntity * entity )
{
    renderGraph->updateEntity( entity );
}

void GraphSystem::removeEntity( ECS::IGameEntity * entity )
{
    renderGraph->removeEntity( entity );
}

void GraphSystem::frameStart( ECS::ComponentTable * componentTable )
{
    renderGraph->prepare( componentTable );
}

void GraphSystem::entityTick( ECS::IGameEntity * entity )
{
}

void GraphSystem::frameEnd( ECS::ComponentTable * componentTable )
{
    FUNCTION_BREAK( !isSystemActive )

    renderGraph->execute( );
}

void GraphSystem::cleanup( )
{
    renderGraph.reset( );
}

END_NAMESPACES