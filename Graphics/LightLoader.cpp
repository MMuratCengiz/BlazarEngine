//
// Created by Murat on 12/2/2020.
//

#include "LightLoader.h"

NAMESPACES( SomeVulkan, Graphics )

void LightLoader::addAmbientLight( const std::shared_ptr< ECS::CAmbientLight >& ambientLight ) {
    ambientLights.push_back( ambientLight );
}

void LightLoader::addDirectionalLight( const std::shared_ptr< ECS::CDirectionalLight >& directionalLight ) {
    directionalLights.push_back( directionalLight );
}

void LightLoader::addPointLight( const std::shared_ptr< ECS::CPointLight >& pointLight ) {
    pointLights.push_back( pointLight );
}

void LightLoader::addSpotLight( const std::shared_ptr< ECS::CSpotLight >& spotLight ) {
    spotLights.push_back( spotLight );
}
/*
void LightLoader::load( ) {
    EnvironmentLights lights{ };

    for ( int i = 0; i < MAX_ALLOWED_LIGHTS; ++i ) {
        if ( ambientLights.size() > i ) {
            lights.ambientLights[ i ].diffuse = glm::vec4( ambientLights[ i ].diffuse, 1.0f);
            lights.ambientLights[ i ].specular = glm::vec4( ambientLights[ i ].specular, 1.0f);
            lights.ambientLights[ i ].power = ambientLights[ i ].power;
        }

        if ( directionalLights.size() > i ) {
            lights.directionalLights[ i ].power = directionalLights[ i ].power;
            lights.directionalLights[ i ].diffuse = glm::vec4( directionalLights[ i ].diffuse, 1.0f);
            lights.directionalLights[ i ].specular = glm::vec4( directionalLights[ i ].specular, 1.0f);
            lights.directionalLights[ i ].direction = glm::vec4( directionalLights[ i ].direction, 1.0f);
        }

        if ( pointLights.size() > i ) {
            lights.pointLights[ i ].attenuationLinear = pointLights[ i ].attenuationLinear;
            lights.pointLights[ i ].attenuationConstant = pointLights[ i ].attenuationConstant;
            lights.pointLights[ i ].attenuationQuadratic = pointLights[ i ].attenuationLinear;
            lights.pointLights[ i ].position = glm::vec4( pointLights[ i ].position, 1.0f);
            lights.pointLights[ i ].diffuse = glm::vec4( pointLights[ i ].diffuse, 1.0f);
            lights.pointLights[ i ].specular = glm::vec4( pointLights[ i ].specular, 1.0f);
        }

        if ( spotLights.size() > i ) {
            lights.spotLights[ i ].power = spotLights[ i ].power;
            lights.spotLights[ i ].radius = spotLights[ i ].radius;
            lights.spotLights[ i ].position = glm::vec4( spotLights[ i ].position, 1.0f);
            lights.spotLights[ i ].direction = glm::vec4( spotLights[ i ].direction, 1.0f);
            lights.spotLights[ i ].diffuse = glm::vec4( spotLights[ i ].diffuse, 1.0f);
            lights.spotLights[ i ].specular = glm::vec4( spotLights[ i ].specular, 1.0f);
        }
    }

    lights.ambientLightCount = ambientLights.size();
    lights.directionalLightCount = directionalLights.size();
    lights.pointLightCount = pointLights.size();
    lights.spotLightCount = spotLights.size();

    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    RenderUtilities::initStagingBuffer( context, stagingBuffer, &lights, sizeof( EnvironmentLights ) );

    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
    bufferCreateInfo.size = sizeof( EnvironmentLights );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo;
    allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    lightBuffers = this->context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    commandExecutor->startCommandExecution()
        ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit )
        ->beginCommand()
        ->copyBuffer( sizeof( EnvironmentLights ), stagingBuffer.first, lightBuffers.first )
        ->execute();

    context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
}*/

void LightLoader::load( ) {
    EnvironmentLights lights{ };

    for ( int i = 0; i < MAX_ALLOWED_LIGHTS; ++i ) {
        if ( ambientLights.size() > i ) {
            lights.ambientLights[ i ].diffuse = glm::vec4( ambientLights[ i ]->diffuse, 1.0f);
            lights.ambientLights[ i ].specular = glm::vec4( ambientLights[ i ]->specular, 1.0f);
            lights.ambientLights[ i ].power = ambientLights[ i ]->power;
        }

        if ( directionalLights.size() > i ) {
            lights.directionalLights[ i ].power = directionalLights[ i ]->power;
            lights.directionalLights[ i ].diffuse = glm::vec4( directionalLights[ i ]->diffuse, 1.0f);
            lights.directionalLights[ i ].specular = glm::vec4( directionalLights[ i ]->specular, 1.0f);
            lights.directionalLights[ i ].direction = glm::vec4( directionalLights[ i ]->direction, 1.0f);
        }

        if ( pointLights.size() > i ) {
            lights.pointLights[ i ].attenuationLinear = pointLights[ i ]->attenuationLinear;
            lights.pointLights[ i ].attenuationConstant = pointLights[ i ]->attenuationConstant;
            lights.pointLights[ i ].attenuationQuadratic = pointLights[ i ]->attenuationLinear;
            lights.pointLights[ i ].position = glm::vec4( pointLights[ i ]->position, 1.0f);
            lights.pointLights[ i ].diffuse = glm::vec4( pointLights[ i ]->diffuse, 1.0f);
            lights.pointLights[ i ].specular = glm::vec4( pointLights[ i ]->specular, 1.0f);
        }

        if ( spotLights.size() > i ) {
            lights.spotLights[ i ].power = spotLights[ i ]->power;
            lights.spotLights[ i ].radius = spotLights[ i ]->radius;
            lights.spotLights[ i ].position = glm::vec4( spotLights[ i ]->position, 1.0f);
            lights.spotLights[ i ].direction = glm::vec4( spotLights[ i ]->direction, 1.0f);
            lights.spotLights[ i ].diffuse = glm::vec4( spotLights[ i ]->diffuse, 1.0f);
            lights.spotLights[ i ].specular = glm::vec4( spotLights[ i ]->specular, 1.0f);
        }
    }

    lights.ambientLightCount = ambientLights.size();
    lights.directionalLightCount = directionalLights.size();
    lights.pointLightCount = pointLights.size();
    lights.spotLightCount = spotLights.size();

    if (! bufferCreated ) {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
        bufferCreateInfo.size = sizeof( EnvironmentLights );
        bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vma::AllocationCreateInfo allocationInfo;
        allocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
        allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
        allocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal;

        lightBuffers = this->context->vma.createBuffer( bufferCreateInfo, allocationInfo );

        memory = context->vma.mapMemory( lightBuffers.second );
        bufferCreated = true;
    }

    memcpy( memory, &lights, sizeof( EnvironmentLights ) );
}

LightLoader::~LightLoader( ) {
    context->vma.unmapMemory( lightBuffers.second );
    context->vma.destroyBuffer( lightBuffers.first, lightBuffers.second );
}

std::pair< vk::Buffer, vma::Allocation > &LightLoader::getBuffer( ) {
    return lightBuffers;
}

END_NAMESPACES