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

#include "ShaderUniformBinder.h"

#include <utility>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

ShaderUniformBinder::ShaderUniformBinder( )
{
    registerBinder(
            "InstanceData",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatInstances( entity->getComponent< ECS::CInstances >( ), entity );
                return getAttachment< InstanceData >( data );
            }
    );

    registerBinder(
            "ModelMatrix",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatModelMatrix( entity->getComponent< ECS::CTransform >( ), entity );
                auto attachment = getAttachment< glm::mat4 >( data );
                attachment->resourceType = ResourceType::PushConstant;
                return attachment;
            }
    );

    registerBinder(
            "NormalModelMatrix",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatNormalMatrix( entity->getComponent< ECS::CTransform >( ), entity );
                auto attachment = getAttachment< glm::mat4 >( data );
                attachment->resourceType = ResourceType::PushConstant;
                return attachment;
            }
    );

    registerBinder(
            "BoneTransformations",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatBoneTransformations( entity );
                return getAttachment< BoneTransformations >( data );
            }
    );

    registerBinder(
            "EnvironmentLights",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatLightingEnvironment( table );
                return getAttachment< EnvironmentLights >( data );
            }
    );

    registerBinder(
            "LightViewProjectionMatrix",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatLightViewProjectionMatrices( table );
                return getAttachment< LightViewProjectionMatrices >( data );
            }
    );

    registerBinder(
            "OutlineColor",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                const auto outlineComponent = entity->getComponent< ECS::COutlined >( );
                return getAttachment< glm::vec4 >( outlineComponent == nullptr ? glm::vec4( 1.0f ) : outlineComponent->outlineColor );
            }
    );

    registerBinder(
            "OutlineScale",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                const auto outlineComponent = entity->getComponent< ECS::COutlined >( );
                return getAttachment< float >( outlineComponent == nullptr ? 1.0f : outlineComponent->borderScale );
            }
    );

    registerBinder(
            "ViewProjection",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatCamera( table );
                return getAttachment< ViewProjection >( data );
            }
    );

    registerBinder(
            "Tessellation",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                ECS::CTessellation * tessellation = entity->getComponent< ECS::CTessellation >( );
                const auto data = DataAttachmentFormatter::formatTessellationComponent( tessellation );
                return getAttachment< Tessellation >( data );
            }
    );

    registerBinder(
            "WorldContext",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                const auto data = DataAttachmentFormatter::formatWorldContext( table );
                return getAttachment< WorldContext >( data );
            }
    );

    registerBinder(
            "Material",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                ECS::CMaterial * material = entity->getComponent< ECS::CMaterial >( );
                const auto data = DataAttachmentFormatter::formatMaterialComponent( entity->getComponent< ECS::CMaterial >( ), entity->getComponent< ECS::CTransform >( ) );
                return getAttachment< Material >( data );
            }
    );

    registerBinder(
            "Resolution",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                const auto gameStateComponent = table->getComponents< ECS::CGameState >( )[ 0 ];
                const auto data = DataAttachmentFormatter::formatResolution( gameStateComponent->surfaceWidth, gameStateComponent->surfaceHeight );
                return getAttachment< Resolution >( data );
            }
    );

    registerBinder(
            "SkyBox",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                return createSamplerShaderUniform( DataAttachmentFormatter::getSkyBoxTextures( table ), ResourceType::CubeMap );
            }
    );

    registerBinder(
            "searchTex",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                return createSamplerShaderUniform( DataAttachmentFormatter::getSearchTex( ) );
            }
    );

    registerBinder(
            "areaTex",
            [ ]( ECS::ComponentTable * table ) -> std::unique_ptr< IShaderUniform >
            {
                return createSamplerShaderUniform( DataAttachmentFormatter::getAreaTex( ) );
            }
    );

    registerBinderLoadOnce(
            "HeightMap",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                return createSamplerShaderUniform( DataAttachmentFormatter::getHeightMap( entity ) );
            }
    );

    registerBinderLoadOnce(
            "Texture1",
            [ ]( ECS::IGameEntity * entity ) -> std::unique_ptr< IShaderUniform >
            {
                return createSamplerShaderUniform( DataAttachmentFormatter::getTexture1( entity ) );
            }
    );
}

void ShaderUniformBinder::registerBinder( std::string uniformName, PerGeometryBinder binder )
{
    AllocatorFunction allocatorFunction = { };
    allocatorFunction.refUniform = uniformName;
    allocatorFunction.frequency = UpdateFrequency::EachGeometry;
    allocatorFunction.perGeometryBinder = std::move( binder );

    registerBinder( std::move( uniformName ), allocatorFunction );
}

void ShaderUniformBinder::registerBinder( std::string uniformName, PerEntityUniformBinder binder )
{
    AllocatorFunction allocatorFunction = { };
    allocatorFunction.refUniform = uniformName;
    allocatorFunction.frequency = UpdateFrequency::EachEntity;
    allocatorFunction.perEntityUniformBinder = std::move( binder );

    registerBinder( std::move( uniformName ), allocatorFunction );
}

void ShaderUniformBinder::registerBinderLoadOnce( std::string uniformName, PerEntityUniformBinder binder )
{
    AllocatorFunction allocatorFunction = { };
    allocatorFunction.refUniform = uniformName;
    allocatorFunction.frequency = UpdateFrequency::Once;
    allocatorFunction.perEntityUniformBinder = std::move( binder );

    int idx = registerBinder( std::move( uniformName ), allocatorFunction );
    allocatorFunction.refIdx = idx;
    loadOnceBinders.push_back( allocatorFunction );
}

void ShaderUniformBinder::registerBinder( std::string uniformName, PerFrameUniformBinder binder )
{
    AllocatorFunction allocatorFunction = { };
    allocatorFunction.refUniform = uniformName;
    allocatorFunction.frequency = UpdateFrequency::EachFrame;
    allocatorFunction.perFrameUniformBinder = std::move( binder );

    registerBinder( std::move( uniformName ), allocatorFunction );
}

int ShaderUniformBinder::registerBinder( std::string uniformName, AllocatorFunction allocator )
{
    const int idx = binders.size( );
    allocator.refIdx = idx;
    binders.push_back( std::move( allocator ) );
    binderIdxMap[ std::move( uniformName ) ] = idx;
    return idx;
}

END_NAMESPACES