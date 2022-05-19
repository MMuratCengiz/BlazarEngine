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

NAMESPACE(ENGINE_NAMESPACE)

class SceneBindings
{
public:
    explicit SceneBindings( sol::state& lua )
    {
        bindWorld( lua );
    }

    void bindWorld( sol::state& lua )
    {
        sol::usertype< Scene::World > worldType = lua.new_usertype< Scene::World >( "World", sol::constructors< Scene::World( ) >( ) );

        worldType["getActionMap"] = &Scene::World::getActionMap;
        worldType["getAssetManager"] = &Scene::World::getAssetManager;
        worldType["getTransformSystem"] = &Scene::World::getTransformSystem;
        worldType["resize"] = &Scene::World::resize;
        worldType["rename"] = &Scene::World::rename;
        worldType["setScene"] = &Scene::World::setScene;
    }
};

END_NAMESPACE