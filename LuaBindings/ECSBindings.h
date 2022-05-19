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
#include <BlazarECS/ECS.h>

NAMESPACE( ENGINE_NAMESPACE )

class ECSBindings
{
    explicit ECSBindings( sol::state &lua )
    {
        bindCAmbientLight( lua );
        bindCAnimState( lua );
        bindCCamera( lua );
        bindCCollisionObject( lua );
        bindCCubeMap( lua );
        bindCDirectionalLight( lua );
        bindCGameState( lua );
        bindCInstances( lua );
        bindCMaterial( lua );
        bindCMesh( lua );
        bindComponentTable( lua );
        bindCOutlined( lua );
        bindCPointLight( lua );
        bindCRigidBody( lua );
        bindCSpotLight( lua );
        bindCTessellation( lua );
        bindCTransform( lua );
        bindECS( lua );
        bindECSUtilities( lua );
        bindIComponent( lua );
        bindIGameEntity( lua );
        bindISystem( lua );
    }

    void bindCAmbientLight( sol::state &lua )
    {
        sol::usertype< ECS::CAmbientLight > worldType = lua.new_usertype< ECS::CAmbientLight >( "World", sol::constructors< ECS::CAmbientLight( ) >( ) );

        worldType["diffuse"] = &ECS::CAmbientLight::diffuse;
        worldType["power"] = &ECS::CAmbientLight::power;
        worldType["specular"] = &ECS::CAmbientLight::specular;
        worldType["uid"] = &ECS::CAmbientLight::uid;
        worldType["typeId"] = &ECS::CAmbientLight::typeId;
    }

    void bindCAnimState( sol::state &lua )
    {
        sol::usertype< ECS::CAnimState > worldType = lua.new_usertype< ECS::CAnimState >( "World", sol::constructors< ECS::CAnimState( ) >( ) );

    }

    void bindCCamera( sol::state &lua )
    {

    }

    void bindCCollisionObject( sol::state &lua )
    {

    }

    void bindCCubeMap( sol::state &lua )
    {

    }

    void bindCDirectionalLight( sol::state &lua )
    {

    }

    void bindCGameState( sol::state &lua )
    {

    }

    void bindCInstances( sol::state &lua )
    {

    }

    void bindCMaterial( sol::state &lua )
    {

    }

    void bindCMesh( sol::state &lua )
    {

    }

    void bindComponentTable( sol::state &lua )
    {

    }

    void bindCOutlined( sol::state &lua )
    {

    }

    void bindCPointLight( sol::state &lua )
    {

    }

    void bindCRigidBody( sol::state &lua )
    {

    }

    void bindCSpotLight( sol::state &lua )
    {

    }

    void bindCTessellation( sol::state &lua )
    {

    }

    void bindCTransform( sol::state &lua )
    {

    }

    void bindECS( sol::state &lua )
    {

    }

    void bindECSUtilities( sol::state &lua )
    {

    }

    void bindIComponent( sol::state &lua )
    {

    }

    void bindIGameEntity( sol::state &lua )
    {

    }

    void bindISystem( sol::state &lua )
    {

    }
};

END_NAMESPACE