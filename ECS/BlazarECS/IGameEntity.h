#pragma once

#include <typeindex>
#include <typeinfo>
#include <BlazarCore/Common.h>
#include "IComponent.h"
#include <mutex>

NAMESPACES( ENGINE_NAMESPACE, ECS )

class IGameEntity
{
private:
    std::unordered_map< std::type_index, std::shared_ptr< IComponent > > componentMap;
    std::vector< std::shared_ptr< IGameEntity > > children;
    uint64_t uid;
public:
    IGameEntity( )
    {
        static std::mutex uidGenLock;
        static uint64_t entityUidCounter = 0;

        uidGenLock.lock( );
        uid = entityUidCounter++;
        uidGenLock.unlock( );
    };

    void addChild( const std::shared_ptr< IGameEntity >& child )
    {
        children.push_back( child );
    }

    const uint64_t &getUID( ) const
    {
        return uid;
    }

    const std::vector< std::shared_ptr< IGameEntity > > &getChildren( ) const
    {
        return children;
    }

    template< class T >
    bool hasComponent( )
    {
        return componentMap.find( typeid( T ) ) != componentMap.end( );
    }

    template< class CastAs >
    std::shared_ptr< CastAs > getComponent( )
    {
        return std::dynamic_pointer_cast< CastAs >( componentMap[ typeid( CastAs ) ] );
    }

    std::vector< std::shared_ptr< IComponent > > getAllComponents( )
    {
        auto result = std::vector< std::shared_ptr< IComponent > >( );

        for ( const auto& pair: componentMap )
        {
            result.push_back( pair.second );
        }

        return result;
    }

    template< class CType >
    std::shared_ptr< CType > createComponent( )
    {
        componentMap[ typeid( CType ) ] = std::dynamic_pointer_cast< IComponent >( std::make_shared< CType >( ) );
        return getComponent< CType >( );
    }

    virtual ~IGameEntity( ) = default;
};

typedef std::shared_ptr< IGameEntity > pGameEntity;

class DynamicGameEntity : public IGameEntity
{
public:
    ~DynamicGameEntity() override = default;
};

END_NAMESPACES