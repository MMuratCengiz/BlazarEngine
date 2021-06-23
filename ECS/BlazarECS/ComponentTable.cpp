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

#include "ComponentTable.h"

void BlazarEngine::ECS::ComponentTable::addAllEntityComponentRecursive( const std::shared_ptr< IGameEntity >& gameEntity )
{
    for ( const auto& component: gameEntity->getAllComponents( ) )
    {
        addNewComponent( component );
    }

    for ( auto child: gameEntity->getChildren( ) )
    {
        addAllEntityComponentRecursive( child );
    }
}

void BlazarEngine::ECS::ComponentTable::addNewComponent( std::shared_ptr< IComponent > component )
{
    auto componentList = componentTable.find( component->typeId );

    if ( componentList == componentTable.end( ) )
    {
        componentTable[ component->typeId ] = { };
        componentList = componentTable.find( component->typeId );
    }

    componentList->second.push_back( std::move( component ) );
}

void BlazarEngine::ECS::ComponentTable::removeComponent( const std::shared_ptr< IComponent > &component )
{
    auto componentList = componentTable.find( component->typeId );

    FUNCTION_BREAK( componentList == componentTable.end( ) ) // Nothing to do

    for ( auto it = componentList->second.begin( ); it != componentList->second.end( ); ++it )
    {
        if ( it->get( )->uid == component->uid )
        {
            componentList->second.erase( it );
        }
    }
}

