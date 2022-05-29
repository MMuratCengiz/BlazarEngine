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

#include <BlazarGraphics/RenderGraph/GraphSystem.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

GraphSystem::GraphSystem( IRenderDevice *renderDevice, AssetManager *assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
    renderGraph = std::make_unique< RenderGraph >( this->renderDevice, this->assetManager );

    Input::Events::subscribe< Input::WindowResizedParameters * >( Input::EventType::WindowResized, [ & ]( Input::WindowResizedParameters * parameters )
    {
        isSystemActive = parameters->width > 0 && parameters->height > 0;
    } );
}

void GraphSystem::addPass( Pass * pass )
{
    passes.push_back( pass );
    buildGraph = true;
}

void GraphSystem::removePass( Pass * pass )
{
    passes.erase(
            std::remove_if(
                    passes.begin(),
                    passes.end(),
                    [&]( Pass * p )
                    {
                        return p->name == pass->name;
                    } ),
            passes.end( )
    );

    buildGraph = true;
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
    if ( buildGraph )
    {
        renderGraph->clearGraph( );
        for ( Pass * p : passes )
        {
            renderGraph->addPass( p );
        }

        renderGraph->buildGraph( );
        buildGraph = false;
    }

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