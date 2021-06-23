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