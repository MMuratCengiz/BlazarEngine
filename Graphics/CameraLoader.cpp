//
// Created by Murat on 11/22/2020.
//

#include "CameraLoader.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

CameraLoader::CameraLoader( std::shared_ptr< InstanceContext > context ) : context( std::move( context ) ) {
    glm::mat4x4 modelMatrix { 1 };

    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    bufferCreateInfo.size = 2 * 4 * 4 * sizeof( float ); // Model matrix size
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo;
    allocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
    allocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal;

    cameraBuffer = this->context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    mappedMemory = this->context->vma.mapMemory( cameraBuffer.second );
}

void CameraLoader::reload( std::shared_ptr< Scene::Camera > pCamera  ) {
    this->camera = std::move( pCamera );
    reload( );
}

void CameraLoader::reload(  ) {
    VP vp{ };
    vp.view = camera->getView();
    vp.projection = camera->getProjection();

    memcpy( mappedMemory, &vp, 2* 4 * 4 * sizeof( float ) );
}

std::pair< vk::Buffer, vma::Allocation > &CameraLoader::getBuffer( ) {
    return cameraBuffer;
}

CameraLoader::~CameraLoader( ) {
    context->vma.unmapMemory( cameraBuffer.second );
    context->vma.destroyBuffer( cameraBuffer.first, cameraBuffer.second );
}

END_NAMESPACES