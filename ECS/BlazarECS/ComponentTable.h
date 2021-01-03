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