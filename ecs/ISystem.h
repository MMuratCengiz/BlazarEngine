#pragma once

#include "../core/Common.h"
#include "../ecs/IGameEntity.h"

NAMESPACES( SomeVulkan, ECS )

template< class ComponentType, class Input >
class ISystem {
protected:
public:
	pComponent& requiredEntityComponent( pGameEntity& entity ) {
		return entity->getComponent< ComponentType >( );
	}

	virtual void beforeFrame( Input& input, const ComponentType& ) { }
	virtual void onFrame(  const ComponentType& ) { }
	virtual void postFrame( const ComponentType& ) { }
};

END_NAMESPACES