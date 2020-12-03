//
// Created by Murat on 12/2/2020.
//

#include "LightLoader.h"

NAMESPACES( SomeVulkan, Graphics )

void LightLoader::addAmbientLight( const ECS::CAmbientLight& ambientLight ) {
    ambientLights.push_back( ambientLight );
}

void LightLoader::addDirectionalLight( const ECS::CDirectionalLight& directionalLight ) {
    directionalLights.push_back( directionalLight );
}

void LightLoader::addPointLight( const ECS::CPointLight& pointLight ) {
    pointLights.push_back( pointLight );
}

void LightLoader::addSpotLight( const ECS::CSpotLight& spotLight ) {
    spotLights.push_back( spotLight );
}

void LightLoader::load( ) {
    EnvironmentLights lights{ };

    for ( int i = 0; i < MAX_ALLOWED_LIGHTS; ++i ) {
        if ( ambientLights.size() > i ) {
            lights.ambientLights[ i ] = ambientLights[ i ];
        }

        if ( directionalLights.size() > i ) {
            lights.directionalLights[ i ] = directionalLights[ i ];
        }

        if ( pointLights.size() > i ) {
            lights.pointLights[ i ] = pointLights[ i ];
        }

        if ( spotLights.size() > i ) {
            lights.spotLights[ i ] = spotLights[ i ];
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
}

LightLoader::~LightLoader( ) {
    context->vma.destroyBuffer( lightBuffers.first, lightBuffers.second );
}

std::pair< vk::Buffer, vma::Allocation > &LightLoader::getBuffer( ) {
    return lightBuffers;
}

END_NAMESPACES