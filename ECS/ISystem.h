#pragma once

#include "../Core/Common.h"
#include "../ECS/IGameEntity.h"

NAMESPACES( SomeVulkan, ECS )

class ISystem {
protected:
public:
	virtual std::vector< std::type_index > getManagedComponents( ) = 0;
    virtual void update( std::shared_ptr< IComponent > component ) = 0;
};

END_NAMESPACES