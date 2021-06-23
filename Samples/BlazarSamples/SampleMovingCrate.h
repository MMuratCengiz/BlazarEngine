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
#include <BlazarInput/GlobalEventHandler.h>
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarPhysics/CollisionShapeInitializer.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleMovingCrate : public ECS::IGameEntity
{
private:
    int order = 0;
    float countSeconds = 0.0f;
    glm::vec3 destination { };
    glm::vec3 currentMovement { };
    glm::mat3 xRotator { };
    glm::mat3 zRotator { };
public:
    SampleMovingCrate( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LightedCube );

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( ECS::Material::TextureInfo { } );
        texInfo.path = PATH( "/assets/textures/container.jpg" );

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 2.0f, 10.0f, 2.0f );
        transform->scale = glm::vec3( 0.5f );

        destination = transform->position;

        xRotator[ 0 ] = glm::vec3( -1.0f, 0.0f, 0.0f );
        xRotator[ 1 ] = glm::vec3( 0.0f,  1.0f, 0.0f );
        xRotator[ 2 ] = glm::vec3( 0.0f,  0.0f, 1.0f );

        zRotator[ 0 ] = glm::vec3( 1.0f, 0.0f, 0.0f );
        zRotator[ 1 ] = glm::vec3( 0.0f, 1.0f, 0.0f );
        zRotator[ 2 ] = glm::vec3( 0.0f, 0.0f, -1.0f );

        Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::Tick, [ & ]( const Input::EventType &event, std::shared_ptr< Input::IEventParameters > parameters )
        {
            countSeconds += Core::Time::getDeltaTime( );

            const std::shared_ptr< ECS::CTransform > &transform = getComponent< ECS::CTransform >( );
            bool hasReachedDestination = glm::length( transform->position - destination ) <= 0.01f;
            if ( !hasReachedDestination )
            {
                transform->position += currentMovement * ( float ) Core::Time::getDeltaTime( );
            }

            if ( hasReachedDestination && countSeconds >= 1.0f )
            {
                transform->position = destination;

                auto& rotator = order == 0 ? xRotator : zRotator;

                destination = rotator * transform->position;;
                countSeconds = 0.0f;

                currentMovement = ( destination - transform->position );
                if ( order == 0 )
                {
                    rotator[ 0 ][ 0 ] *= -1;
                }
                else
                {
                    rotator[ 2 ][ 2 ] *= -1;
                }

                order = order == 0 ? 1 : 0;
            }
        } );
    }
};

}