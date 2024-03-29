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
#include "IResourceProvider.h"
#include <array>
#include <BlazarCore/Utilities.h>
#include "RenderGraph/AreaTex.h"
#include "RenderGraph/SearchTex.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

#define MAX_ALLOWED_LIGHTS 16

struct AmbientLight
{
    alignas( 4 ) float power;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
};

struct Material
{
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
    alignas( 16 ) glm::vec4 textureScale;
    alignas( 4 ) float shininess;
    alignas( 4 ) uint32_t hasHeightMap;
};

struct Tessellation
{
    alignas( 4 ) float innerLevel;
    alignas( 4 ) float outerLevel;
};

struct DirectionalLight
{
    alignas( 4 ) float power;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
    alignas( 16 ) glm::vec4 direction;
};

struct PointLight
{
    alignas( 4 ) float attenuationConstant;
    alignas( 4 ) float attenuationLinear;
    alignas( 4 ) float attenuationQuadratic;
    alignas( 16 ) glm::vec4 position;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
};

struct SpotLight
{
    alignas( 4 ) float power;
    alignas( 4 ) float radius;
    alignas( 16 ) glm::vec4 position;
    alignas( 16 ) glm::vec4 direction;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
};

struct EnvironmentLights
{
    alignas( 4 ) int ambientLightCount { };
    alignas( 4 ) int directionalLightCount { };
    alignas( 4 ) int pointLightCount { };
    alignas( 4 ) int spotLightCount { };

    alignas( 16 ) AmbientLight ambientLights[MAX_ALLOWED_LIGHTS] { };
    alignas( 16 ) DirectionalLight directionalLights[MAX_ALLOWED_LIGHTS] { };
    alignas( 16 ) PointLight pointLights[MAX_ALLOWED_LIGHTS] { };
    alignas( 16 ) SpotLight spotLights[MAX_ALLOWED_LIGHTS] { };
};

struct LightViewProjectionMatrices
{
    glm::mat4 data[ 3 ];
    int count;
};

struct ViewProjection
{
    glm::mat4 view;
    glm::mat4 projection;
};

struct InstanceData
{
    glm::mat4 instances[ 100 ];
    uint32_t instanceCount;
};

struct Resolution
{
    uint32_t width;
    uint32_t height;
};

struct BoneTransformations
{
    glm::mat4 data[ 100 ];
    unsigned int size;
};

struct WorldContext
{
    glm::vec4 cameraPosition;
};

class DataAttachmentFormatter
{
public:
    static Material formatMaterialComponent( ECS::CMaterial * material, ECS::CTransform * transform );
    static Tessellation formatTessellationComponent( ECS::CTessellation * tessellation );
    static ViewProjection formatCamera( ECS::ComponentTable* components );
    static EnvironmentLights formatLightingEnvironment( ECS::ComponentTable* components );
    static LightViewProjectionMatrices formatLightViewProjectionMatrices( ECS::ComponentTable* components );
    static WorldContext formatWorldContext( ECS::ComponentTable* components );
    static glm::mat4 formatModelMatrix( ECS::CTransform * transform, ECS::IGameEntity * refEntity );
    static glm::mat4 formatNormalMatrix( ECS::CTransform * transform, ECS::IGameEntity * refEntity );
    static InstanceData formatInstances( ECS::CInstances * instances, ECS::IGameEntity* entity );
    static BoneTransformations formatBoneTransformations( ECS::IGameEntity * entity );
    static Resolution formatResolution( const uint32_t& width, const uint32_t& height );
    static std::vector< ECS::Material::TextureInfo > getSkyBoxTextures( ECS::ComponentTable* components );
    static std::vector< ECS::Material::TextureInfo > getSearchTex( );
    static std::vector< ECS::Material::TextureInfo > getAreaTex( );
    static std::vector< ECS::Material::TextureInfo > getHeightMap( ECS::IGameEntity* entity );
    static std::vector< ECS::Material::TextureInfo > getTexture1( ECS::IGameEntity* entity );
};

END_NAMESPACES