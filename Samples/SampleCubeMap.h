#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"
#include "../ECS/CCubeMap.h"
#include "../Graphics/BuiltinPrimitives.h"

using namespace BlazarEngine;

namespace Sample
{

class SampleCubeMap : public ECS::IGameEntity
{
public:
    SampleCubeMap( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::PlainCube );

        auto cubeMap = createComponent< ECS::CCubeMap >( );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Right,
                "/assets/cubemaps/greensky/right.png" } );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Left,
                "/assets/cubemaps/greensky/left.png" } );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Front,
                "/assets/cubemaps/greensky/front.png" } );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Back,
                "/assets/cubemaps/greensky/back.png" } );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Top,
                "/assets/cubemaps/greensky/top.png" } );

        cubeMap->texturePaths.push_back( ECS::CubeMapSidePath {
                ECS::CubeMapSide::Bottom,
                "/assets/cubemaps/greensky/bottom.png" } );
    }
};

}
