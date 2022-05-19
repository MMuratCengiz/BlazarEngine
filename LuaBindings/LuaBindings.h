// Blazar Engine - 3D Game Engine
// Copyright (c) 2020-2021 Muhammed Murat Cengiz
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <BlazarCore/Common.h>
#include "BindingsCommon.h"
#include "BlazarScene/World.h"
#include "CoreBindings.h"
#include "ECSBindings.h"
#include "GraphicsBindings.h"
#include "InputBindings.h"
#include "PhysicsBindings.h"
#include "SceneBindings.h"

NAMESPACE( ENGINE_NAMESPACE )

class LuaBindings
{
private:
    sol::state lua;
public:
    LuaBindings( Scene::World *world )
    {
        lua.open_libraries( sol::lib::base, sol::lib::package );

        bindAll( );
    }

    void bindAll( )
    {
        SceneBindings sceneBindings { lua };
        ECSBindings ecsBindings { lua };
    }
};

END_NAMESPACE