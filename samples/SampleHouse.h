#pragma once

#include "../ecs/IGameEntity.h"
#include "../ecs/CMesh.h"
#include "../ecs/CMaterial.h"

class SampleHouse : public IGameEntity {
	SampleHouse( ) {
		auto& material = createComponent< ECS::CMaterial >( );
		auto& mesh = createComponent< ECS::CMesh>( );

		mesh->path = PATH( );
	}
};