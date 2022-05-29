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
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarPhysics/CollisionShapeInitializer.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleCrate : public ECS::IGameEntity
{
public:
    SampleCrate( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LitCube );

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( ECS::Material::TextureInfo { } );
        texInfo.path = PATH( "/assets/textures/container.jpg" );

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 200.0f, -2.8f );
        transform->scale = glm::vec3( 1.0f );
        transform->rotation.euler = glm::vec3( 46.0f, 0.0f, 0.0f );

        auto rigidBody = createComponent< ECS::CRigidBody >( );
        rigidBody->mass = 0.2f;

        Physics::CollisionShapeInitializer( rigidBody, transform ).initializeBoxCollisionShape( transform->scale );
    }
};

}