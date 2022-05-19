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

 You should have received a copy of the GNU General Public Licensealong with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "DataAttachmentFormatter.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

ViewProjection BlazarEngine::Graphics::DataAttachmentFormatter::formatCamera( ECS::ComponentTable * components )
{
    ViewProjection vp { };

    bool oneFound = false;
    for ( const auto &camera : components->getComponents< ECS::CCamera >( ) )
    {
        if ( camera->isActive )
        {
            oneFound = true;
            vp.view = camera->view;
            vp.projection = camera->projection;
        }
    }

    ASSERT_M( oneFound, "At least one camera should be present in the scene and set to active!" );

    return vp;
}

EnvironmentLights DataAttachmentFormatter::formatLightingEnvironment( ECS::ComponentTable * components )
{
    const auto &ambientLights = components->getComponents< ECS::CAmbientLight >( );
    const auto &directionalLights = components->getComponents< ECS::CDirectionalLight >( );
    const auto &pointLights = components->getComponents< ECS::CPointLight >( );
    const auto &spotLights = components->getComponents< ECS::CSpotLight >( );

    EnvironmentLights lights { };

    uint32_t i = 0;
    for ( const auto &ambientLight : ambientLights )
    {
        lights.ambientLights[ i ].diffuse = glm::vec4( ambientLight->diffuse, 1.0f );
        lights.ambientLights[ i ].specular = glm::vec4( ambientLight->specular, 1.0f );
        lights.ambientLights[ i ].power = ambientLight->power;
        ++i;
    }

    i = 0;
    for ( const auto &directionalLight : directionalLights )
    {
        lights.directionalLights[ i ].power = directionalLight->power;
        lights.directionalLights[ i ].diffuse = glm::vec4( directionalLight->diffuse, 1.0f );
        lights.directionalLights[ i ].specular = glm::vec4( directionalLight->specular, 1.0f );
        lights.directionalLights[ i ].direction = glm::vec4( directionalLight->direction, 1.0f );
        ++i;
    }

    i = 0;
    for ( const auto &pointLight : pointLights )
    {
        lights.pointLights[ i ].attenuationLinear = pointLight->attenuationLinear;
        lights.pointLights[ i ].attenuationConstant = pointLight->attenuationConstant;
        lights.pointLights[ i ].attenuationQuadratic = pointLight->attenuationLinear;
        lights.pointLights[ i ].position = glm::vec4( pointLight->position, 1.0f );
        lights.pointLights[ i ].diffuse = glm::vec4( pointLight->diffuse, 1.0f );
        lights.pointLights[ i ].specular = glm::vec4( pointLight->specular, 1.0f );
        ++i;
    }

    i = 0;
    for ( const auto &spotLight : spotLights )
    {
        lights.spotLights[ i ].power = spotLight->power;
        lights.spotLights[ i ].radius = spotLight->radius;
        lights.spotLights[ i ].position = glm::vec4( spotLight->position, 1.0f );
        lights.spotLights[ i ].direction = glm::vec4( spotLight->direction, 1.0f );
        lights.spotLights[ i ].diffuse = glm::vec4( spotLight->diffuse, 1.0f );
        lights.spotLights[ i ].specular = glm::vec4( spotLight->specular, 1.0f );
        ++i;
    }


    lights.ambientLightCount = ambientLights.size( );
    lights.directionalLightCount = directionalLights.size( );
    lights.pointLightCount = pointLights.size( );
    lights.spotLightCount = spotLights.size( );

    return lights;
}

glm::mat4 DataAttachmentFormatter::formatModelMatrix( ECS::CTransform * transform, ECS::IGameEntity * refEntity )
{
    glm::vec3 radiansRotation = transform->rotation.euler;

    if ( transform->rotation.rotationUnit == ECS::RotationUnit::Degrees )
    {
        radiansRotation = glm::vec3(
                glm::radians( transform->rotation.euler.x ),
                glm::radians( transform->rotation.euler.y ),
                glm::radians( transform->rotation.euler.z )
        );
    }

    glm::quat qRotation { radiansRotation };

    return Core::Utilities::getTRSMatrix( transform->position, qRotation, transform->scale );
}

glm::mat4 DataAttachmentFormatter::formatNormalMatrix( ECS::CTransform * transform, ECS::IGameEntity * refEntity )
{
    glm::mat3 normalMatrix = glm::mat3( formatModelMatrix( transform, refEntity ) );

    normalMatrix = glm::inverse( normalMatrix );
    normalMatrix = glm::transpose( normalMatrix );

    return { normalMatrix };
}

Material DataAttachmentFormatter::formatMaterialComponent( ECS::CMaterial * material, ECS::CTransform * transform )
{
    if ( material == nullptr )
    {
        return { };
    }

    return
            {
                    material->diffuse,
                    material->specular,
                    glm::vec4( material->textureScale, 1.0f ),
                    material->shininess,
                    ( uint32_t ) ( material->heightMap.path.empty( ) ? 0 : 1 )
            };
}

Tessellation DataAttachmentFormatter::formatTessellationComponent( ECS::CTessellation * tessellation )
{
    if ( tessellation == nullptr )
    {
        return { };
    }

    return { tessellation->innerLevel, tessellation->outerLevel };
}

InstanceData DataAttachmentFormatter::formatInstances( ECS::CInstances * instances, ECS::IGameEntity * entity )
{
    InstanceData instanceData = { };
    instanceData.instanceCount = 0;

    if ( instances == nullptr )
    {
        return instanceData;
    }

    uint32_t i = 0;

    for ( ; i < instances->transforms.size( ); ++i )
    {
        instanceData.instances[ i ] = formatModelMatrix( instances->transforms[ i ], entity );
    }
   
    instanceData.instanceCount = i;

    return instanceData;
}

Resolution DataAttachmentFormatter::formatResolution( const uint32_t& width, const uint32_t& height )
{
    return Resolution{ width, height };
}

BoneTransformations DataAttachmentFormatter::formatBoneTransformations( ECS::IGameEntity * entity )
{
    BoneTransformations boneTransformations = { };

    auto animState = entity->getComponent< ECS::CAnimState >();

    if ( animState == nullptr )
    {
        return boneTransformations;
    }

    for ( int i = 0; i < animState->boneTransformations.size( ); ++i )
    {
        boneTransformations.data[ i ] = animState->boneTransformations[ i ];
    }

    boneTransformations.size = animState->boneTransformations.size( );

    return boneTransformations;
}

LightViewProjectionMatrices DataAttachmentFormatter::formatLightViewProjectionMatrices( ECS::ComponentTable * table )
{
    const auto directionalLights = table->getComponents< ECS::CDirectionalLight >( );

    LightViewProjectionMatrices result = { };

    uint32_t i = 0;

    for ( const auto &light: directionalLights )
    {
        glm::mat4 lightProjection = glm::ortho( -100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 50.0f );

        lightProjection = VK_CORRECTION_MATRIX * lightProjection;

        glm::vec3 pos = glm::normalize( -light->direction ) * 30.0f;
        glm::vec3 front = glm::vec3( 0, 0, 0 );

        glm::mat4 lightView = glm::lookAt( pos, front, glm::vec3( 0.0f, 1.0f, 0.0f ) );

        result.data[ i ] = lightProjection * lightView;

        if ( i++ == 3 )
        {
            break;
        }
    }

    result.count = i;
    return result;
}

WorldContext DataAttachmentFormatter::formatWorldContext( ECS::ComponentTable * table )
{
    auto cameras = table->getComponents< ECS::CCamera >( );

    auto activeCamera = cameras[ 0 ];

    int i = 0;
    while ( !activeCamera->isActive )
    {
        ASSERT_M( i < cameras.size( ), "You must have a single active camera." );
        activeCamera = cameras[ ++i ];
    }

    WorldContext data { };
    data.cameraPosition = glm::vec4( activeCamera->position, 1.0f );
    return data;
}

std::vector< ECS::Material::TextureInfo > DataAttachmentFormatter::getSkyBoxTextures( ECS::ComponentTable * table )
{
    std::vector< ECS::Material::TextureInfo > result;

    for ( const auto& cubeMap : table->getComponents< ECS::CCubeMap >( ) )
    {
        for ( const auto& texture: cubeMap->texturePaths )
        {
            result.push_back( ECS::Material::TextureInfo { texture.path } );
        }
    }

    return result;
}

std::vector< ECS::Material::TextureInfo > DataAttachmentFormatter::getSearchTex( )
{
    std::vector< ECS::Material::TextureInfo > result;

    auto &searchTextInfo = result.emplace_back( ECS::Material::TextureInfo { } );

    searchTextInfo.U = ECS::Material::AddressMode::ClampToEdge;
    searchTextInfo.V = ECS::Material::AddressMode::ClampToEdge;
    searchTextInfo.W = ECS::Material::AddressMode::ClampToEdge;

    searchTextInfo.isInMemory = true;
    searchTextInfo.inMemoryTexture = { };
    searchTextInfo.inMemoryTexture.contents = searchTexBytes;
    searchTextInfo.inMemoryTexture.width = SEARCHTEX_WIDTH;
    searchTextInfo.inMemoryTexture.height = SEARCHTEX_HEIGHT;
    searchTextInfo.inMemoryTexture.channels = 4;
    searchTextInfo.inMemoryTexture.format = ECS::Material::ImageFormat::R8Unorm;

    return result;
}

std::vector< ECS::Material::TextureInfo > DataAttachmentFormatter::getAreaTex( )
{
    std::vector< ECS::Material::TextureInfo > result;

    auto &areaTexInfo = result.emplace_back( ECS::Material::TextureInfo { } );

    areaTexInfo.U = ECS::Material::AddressMode::ClampToEdge;
    areaTexInfo.V = ECS::Material::AddressMode::ClampToEdge;
    areaTexInfo.W = ECS::Material::AddressMode::ClampToEdge;

    areaTexInfo.isInMemory = true;
    areaTexInfo.inMemoryTexture = { };
    areaTexInfo.inMemoryTexture.contents = areaTexBytes;
    areaTexInfo.inMemoryTexture.width = AREATEX_WIDTH;
    areaTexInfo.inMemoryTexture.height = AREATEX_HEIGHT;
    areaTexInfo.inMemoryTexture.channels = 4;
    areaTexInfo.inMemoryTexture.format = ECS::Material::ImageFormat::R8G8Unorm;

    return result;
}

std::vector< ECS::Material::TextureInfo > DataAttachmentFormatter::getHeightMap( ECS::IGameEntity * entity )
{
    std::vector< ECS::Material::TextureInfo > result;

    ECS::CMaterial * material = entity->getComponent< ECS::CMaterial >( );

    if ( material != nullptr && !material->heightMap.path.empty( ) )
    {
        result.push_back( material->heightMap );
    }

    return result;
}

std::vector< ECS::Material::TextureInfo > DataAttachmentFormatter::getTexture1( ECS::IGameEntity * entity )
{
    std::vector< ECS::Material::TextureInfo > result;

    ECS::CMaterial * material = entity->getComponent< ECS::CMaterial >( );

    if ( material != nullptr && !material->textures[ 0 ].path.empty( ) )
    {
        result.push_back( material->textures[ 0 ] );
    }

    return result;
}

END_NAMESPACES