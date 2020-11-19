#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, ECS )

struct IComponent {
	IComponent( ) = default;
	virtual ~IComponent( ) { }
};

typedef std::shared_ptr< IComponent > pComponent;

END_NAMESPACES