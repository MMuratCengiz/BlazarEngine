/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

		componentTable->addAllEntityComponentRecursive( entity );
		entities.emplace_back( std::move( entity ) );
	}

	[[nodiscard]] const std::vector< std::shared_ptr< ECS::IGameEntity > >& getEntities( ) const
	{
		return entities;
	}

	[[nodiscard]] const std::shared_ptr< ECS::ComponentTable >& getComponentTable( ) const
	{
		return componentTable;
	}
};

END_NAMESPACES
