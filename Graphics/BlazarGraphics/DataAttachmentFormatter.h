#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include "IResourceProvider.h"

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

struct ViewProjection
{
    glm::mat4 view;
    glm::mat4 projection;
};

class DataAttachmentFormatter
{
public:
    static Material formatMaterialComponent( const std::shared_ptr< ECS::CMaterial > &material, const std::shared_ptr< ECS::CTransform > &transform );
    static ViewProjection formatCamera( const std::shared_ptr< ECS::ComponentTable > &components );
    static EnvironmentLights formatLightingEnvironment( const std::shared_ptr< ECS::ComponentTable > &components );
    static glm::mat4 formatModelMatrix( const std::shared_ptr< ECS::CTransform > &transform );
    static glm::mat4 formatNormalMatrix( const std::shared_ptr< ECS::CTransform > &transform );

};

END_NAMESPACES