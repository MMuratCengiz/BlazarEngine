#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

#include "BlazarECS/CCamera.h"

NAMESPACES( ENGINE_NAMESPACE, Scene )

class Scene
{
private:
    std::shared_ptr< ECS::ComponentTable > componentTable;

    std::vector< std::shared_ptr< ECS::IGameEntity > > entities;
public:
    Scene( )
    {
        componentTable = std::make_shared< ECS::ComponentTable >( );
    }

    inline void addEntity( std::shared_ptr< ECS::IGameEntity > entity )
    {
        NOT_NULL( entity );

        for ( const auto& component: entity->getAllComponents( ) )
        {
            componentTable->addNewComponent( component );
        }

        entities.emplace_back( std::move( entity ) );
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::IGameEntity > > &getEntities( ) const
    {
        return entities;
    }

    [[nodiscard]] const std::shared_ptr< ECS::ComponentTable > &getComponentTable( ) const
    {
        return componentTable;
    }
};

END_NAMESPACES
