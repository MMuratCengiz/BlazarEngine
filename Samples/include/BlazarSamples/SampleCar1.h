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
#include <BlazarPhysics/PhysicsTransformSystem.h>

namespace Sample
{

class SampleCar1 : public BlazarEngine::ECS::IGameEntity
{
private:
    std::unique_ptr< ECS::IGameEntity > meshEntities;
public:
    explicit SampleCar1( Scene::World * world )
    {
        meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/car_1.gltf" ) );

        for ( auto& child: meshEntities->getChildren( ) )
        {
            child->getComponent< BlazarEngine::ECS::CMaterial >( )->shininess = 1.0f;
            child->getComponent< BlazarEngine::ECS::CMesh >( )->cullMode = BlazarEngine::ECS::CullMode::None;
            auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
            texInfo.path = PATH( "/assets/textures/Car Texture 1.png" );
        }

        addChild( meshEntities.get( ) );

        BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( this, glm::vec3( 3.0f, 0.15f, 5.0f ) );
        BlazarEngine::Physics::PhysicsTransformSystem::setRotationRecursive( this, { BlazarEngine::ECS::RotationUnit::Degrees, glm::vec3( -0.0f, -45.0f, 0.0f ) } );
        BlazarEngine::Physics::PhysicsTransformSystem::setScaleRecursive( this, glm::vec3( 0.5f, 0.5f, 0.5f ));
    }
};

}