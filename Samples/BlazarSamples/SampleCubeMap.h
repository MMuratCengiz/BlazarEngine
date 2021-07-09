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

#include <BlazarECS/ECS.h>
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarScene/World.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleCubeMap : public ECS::IGameEntity
{
public:
    SampleCubeMap( Scene::World * world )
    {
        world->getAssetManager( )->createEntity( this, Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::PlainCube ) );

        auto cubeMap = createComponent< ECS::CCubeMap >( );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Right,
        PATH( "/assets/cubemaps/greensky/right.png" } ) );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Left,
        PATH( "/assets/cubemaps/greensky/left.png" } ) );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Front,
        PATH( "/assets/cubemaps/greensky/front.png" } ) );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Back,
        PATH( "/assets/cubemaps/greensky/back.png" } ) );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Top,
        PATH( "/assets/cubemaps/greensky/top.png" } ) );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Bottom,
        PATH( "/assets/cubemaps/greensky/bottom.png" } ) );

        cubeMap->sort( );
    }
};

}
