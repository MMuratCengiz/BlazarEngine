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
#include <typeindex>
#include "IGameEntity.h"
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

class ComponentTable
{
private:
    /* Used Table Structure */
    std::vector<
            std::vector < // dynamically indexed componentId != index
                IComponent * > > componentTable;
public:
    void addAllEntityComponentRecursive( IGameEntity * gameEntity );
    void addNewComponent( IComponent * component );
    // Expensive operation don't use often
    void removeComponent( IComponent * component );

    template< class ComponentType >
    inline std::vector< ComponentType * > getComponents( )
    {
        auto result = std::vector< ComponentType * >( );

        const uint64_t typeId = ComponentTypeRef::get( ).getTypeId< ComponentType >( );

        if ( typeId >= componentTable.size( ) )
        {
            return result;
        }

        auto componentList = componentTable[ typeId ];

        for ( auto& component: componentList )
        {
            auto castedComponent = ( ComponentType * )( component );
            result.push_back( castedComponent );
        }

        return result;
    }
};

END_NAMESPACES