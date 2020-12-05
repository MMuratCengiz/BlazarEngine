#pragma once

#include <typeindex>
#include <typeinfo>
#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES(ENGINE_NAMESPACE, ECS)

class IGameEntity {
private:
    std::unordered_map< std::type_index, std::shared_ptr< IComponent > > componentMap;
public:
    IGameEntity() = default;

    template< class T >
    bool hasComponent( ) {
        return componentMap.find( typeid( T )) != componentMap.end();
    }

    template< class CastAs >
    std::shared_ptr< CastAs > getComponent( ) {
        return std::dynamic_pointer_cast< CastAs >( componentMap[ typeid( CastAs ) ] );
    }

    template< class CType >
    std::shared_ptr< CType > createComponent( ) {
        componentMap[ typeid( CType ) ] = std::dynamic_pointer_cast< IComponent >( std::make_shared< CType >( ) );
        return getComponent< CType >();
    }

    virtual ~IGameEntity( ) = default;
};

typedef std::shared_ptr< IGameEntity > pGameEntity;

END_NAMESPACES