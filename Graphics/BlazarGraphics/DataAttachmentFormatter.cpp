#include "DataAttachmentFormatter.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

ViewProjection BlazarEngine::Graphics::DataAttachmentFormatter::formatCamera( const std::shared_ptr< ECS::ComponentTable > &components )
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

    ASSERT_M( oneFound, "At least one camera should be present in the scene and set to active!");

    return vp;
}

EnvironmentLights DataAttachmentFormatter::formatLightingEnvironment( const std::shared_ptr< ECS::ComponentTable > &components )
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

glm::mat4 DataAttachmentFormatter::formatModelMatrix( const std::shared_ptr< ECS::CTransform > &transform )
{
    glm::mat4 modelMatrix { 1 };

    modelMatrix = glm::translate( modelMatrix, transform->position );
    modelMatrix = glm::scale( modelMatrix, transform->scale );

    glm::vec3 radiansRotation = transform->rotation.euler;

    if ( transform->rotation.rotationUnit == ECS::RotationUnit::Degrees )
    {
        radiansRotation = glm::vec3(
                glm::radians( transform->rotation.euler.x ),
                glm::radians( transform->rotation.euler.y ),
                glm::radians( transform->rotation.euler.z )
        );
    }

    glm::qua qRotation { radiansRotation };
    modelMatrix *= glm::mat4_cast( qRotation );

    return modelMatrix;
}

Material DataAttachmentFormatter::formatMaterialComponent( const std::shared_ptr< ECS::CMaterial > &material, const std::shared_ptr< ECS::CTransform > &transform )
{
    glm::vec4 textureScale;

    textureScale.x = material->textureScaleOptions.scaleX ? transform->scale.x : 1.0f;
    textureScale.y = material->textureScaleOptions.scaleX ? transform->scale.y : 1.0f;
    textureScale.z = material->textureScaleOptions.scaleX ? transform->scale.z : 1.0f;

    return {
        material->diffuse,
        material->specular,
        textureScale,
        material->shininess
    };
}

END_NAMESPACES