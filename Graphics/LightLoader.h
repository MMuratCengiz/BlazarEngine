#pragma once

#include "../Core/Common.h"
#include "../ECS/ISystem.h"
#include "../ECS/CAmbientLight.h"
#include "../ECS/CDirectionalLight.h"
#include "../ECS/CSpotLight.h"
#include "../ECS/CPointLight.h"

#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"

NAMESPACES( SomeVulkan, Graphics )

#define MAX_ALLOWED_LIGHTS 16

class LightLoader {
private:
    struct EnvironmentLights {
        alignas( 4 ) int ambientLightCount { };
        alignas( 4 ) int directionalLightCount { };
        alignas( 4 ) int pointLightCount { };
        alignas( 4 ) int spotLightCount { };

        alignas( 16 ) ECS::CAmbientLight ambientLights[MAX_ALLOWED_LIGHTS];
        alignas( 16 ) ECS::CDirectionalLight directionalLights[MAX_ALLOWED_LIGHTS];
        alignas( 16 ) ECS::CPointLight pointLights[MAX_ALLOWED_LIGHTS];
        alignas( 16 ) ECS::CSpotLight spotLights[MAX_ALLOWED_LIGHTS];
    };

    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< CommandExecutor > commandExecutor;

    std::vector< ECS::CAmbientLight > ambientLights;
    std::vector< ECS::CDirectionalLight > directionalLights;
    std::vector< ECS::CPointLight > pointLights;
    std::vector< ECS::CSpotLight > spotLights;

    std::pair< vk::Buffer, vma::Allocation > lightBuffers;
public:
    inline explicit LightLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< CommandExecutor > commandExecutor )
            : context( std::move( context ) ), commandExecutor( std::move( commandExecutor ) ) { }

    void addAmbientLight( const ECS::CAmbientLight &ambientLight );
    void addDirectionalLight( const ECS::CDirectionalLight &directionalLight );
    void addPointLight( const ECS::CPointLight &pointLight );
    void addSpotLight( const ECS::CSpotLight &spotLight );

    void load( );

    std::pair< vk::Buffer, vma::Allocation > &getBuffer( );
    ~LightLoader( );
};

END_NAMESPACES