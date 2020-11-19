#pragma once

#include "../core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CMesh: public IComponent {
	std::string path;
};

END_NAMESPACES