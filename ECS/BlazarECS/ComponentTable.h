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
    std::unordered_map< std::type_index,
            std::vector < // dynamically indexed componentId != index
                std::shared_ptr< IComponent > > > componentTable;
public:
    void addAllEntityComponentRecursive( const std::shared_ptr< IGameEntity >& gameEntity );
    void addNewComponent( std::shared_ptr< IComponent > component );
    // Expensive operation don't use often
    void removeComponent( const std::shared_ptr< IComponent >& component );

    template< class ComponentType >
    inline std::vector< std::shared_ptr< ComponentType > > getComponents( )
    {
        auto result = std::vector< std::shared_ptr< ComponentType > >( );

        auto componentList = componentTable.find( typeid( ComponentType ) );

        if ( componentList == componentTable.end( ) )
        {
            return result;
        }

        for ( auto& component: componentList->second )
        {
            auto castedComponent = std::dynamic_pointer_cast< ComponentType >( component );
            result.push_back( castedComponent );
        }

        return result;
    }

    template< class ComponentType >
    inline static std::type_index getComponentTypeId( ) {
        return typeid( ComponentType );
    }
};

END_NAMESPACES