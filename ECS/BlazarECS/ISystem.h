#pragma once

#include <BlazarCore/Common.h>
#include "IGameEntity.h"
#include "ComponentTable.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

class ISystem
{
protected:
public:
    virtual void addEntity( const std::shared_ptr< IGameEntity >& entity ) { };
    virtual void updateEntity( const std::shared_ptr< IGameEntity >& entity ) { };
    virtual void removeEntity( const std::shared_ptr< IGameEntity >& entity ) { };

    virtual void frameStart( const std::shared_ptr< ComponentTable >& componentTable ) = 0;
    virtual void entityTick( const std::shared_ptr< IGameEntity >& entity ) = 0;
    virtual void frameEnd( const std::shared_ptr< ComponentTable >& componentTable ) = 0;
    // Necessary due to some circular dependencies within systems
    virtual void cleanup( ) = 0;
    virtual ~ISystem() = default;
};

END_NAMESPACES