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

#include <BlazarECS/ComponentTable.h>

void BlazarEngine::ECS::ComponentTable::addAllEntityComponentRecursive( IGameEntity * gameEntity )
{
    for ( const auto& component: gameEntity->getAllComponents( ) )
    {
        addNewComponent( component );
    }

    for ( const auto &child: gameEntity->getChildren( ) )
    {
        addAllEntityComponentRecursive( child );
    }
}

void BlazarEngine::ECS::ComponentTable::addNewComponent( IComponent * component )
{
    if ( component->typeId >= componentTable.size( ) )
    {
        componentTable.resize( component->typeId + 1 );
        componentTable[ component->typeId ] = { };
    }
  
    componentTable[ component->typeId ].push_back( std::move( component ) );
}

void BlazarEngine::ECS::ComponentTable::removeComponent( IComponent * component )
{
    FUNCTION_BREAK( component->typeId >= componentTable.size(  ) )

    auto componentList = componentTable[ component->typeId ];

    for ( auto it = componentList.begin( ); it != componentList.end( ); ++it )
    {
        if ( (*it)->uid == component->uid )
        {
            componentList.erase( it );
        }
    }
}

