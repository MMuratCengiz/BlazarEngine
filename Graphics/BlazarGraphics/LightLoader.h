#pragma once

#include <BlazarECS/ECS.h>
#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

#define MAX_ALLOWED_LIGHTS 16

class LightLoader
{
private:
    struct AmbientLight
    {
        alignas( 4 ) float power;
        alignas( 16 ) glm::vec4 diffuse;
        alignas( 16 ) glm::vec4 specular;
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

    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< CommandExecutor > commandExecutor;

    std::vector< std::shared_ptr< ECS::CAmbientLight > > ambientLights;
    std::vector< std::shared_ptr< ECS::CDirectionalLight > > directionalLights;
    std::vector< std::shared_ptr< ECS::CPointLight > > pointLights;
    std::vector< std::shared_ptr< ECS::CSpotLight > > spotLights;

    void *memory;
    bool bufferCreated = false;
    std::pair< vk::Buffer, vma::Allocation > lightBuffers;
public:
    inline explicit LightLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< CommandExecutor > commandExecutor )
            : context( std::move( context ) ), commandExecutor( std::move( commandExecutor ) )
    { }

    void addAmbientLight( const std::shared_ptr< ECS::CAmbientLight > &ambientLight );
    void addDirectionalLight( const std::shared_ptr< ECS::CDirectionalLight > &directionalLight );
    void addPointLight( const std::shared_ptr< ECS::CPointLight > &pointLight );
    void addSpotLight( const std::shared_ptr< ECS::CSpotLight > &spotLight );

    void load( );

    std::pair< vk::Buffer, vma::Allocation > &getBuffer( );
    ~LightLoader( );
};

END_NAMESPACES