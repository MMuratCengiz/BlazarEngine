//
// Created by Murat on 11/21/2020.
//

#include "TransformLoader.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

NAMESPACES( SomeVulkan, Graphics )

TransformLoader::TransformLoader( std::shared_ptr< InstanceContext > context ) : context( std::move( context ) ) {
    glm::mat4x4 modelMatrix { 1 };

    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    bufferCreateInfo.size = 4 * 4 * sizeof( float ); // Model matrix size
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo;
    allocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
    allocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal;

    transformBuffer = this->context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    mappedMemory =  this->context->vma.mapMemory( transformBuffer.second );
}

void TransformLoader::load( const std::shared_ptr< ECS::CTransform > &transform ) {
    glm::mat4 modelMatrix { 1 };

    modelMatrix = glm::translate( modelMatrix, transform->position );
    modelMatrix = glm::scale( modelMatrix, transform->scale );

    glm::vec3 radiansRotation = transform->rotation.euler;

    if ( transform->rotation.rotationUnit == ECS::RotationUnit::Degrees ) {
        radiansRotation = glm::vec3(
                glm::radians( transform->rotation.euler.x ),
                glm::radians( transform->rotation.euler.y ),
                glm::radians( transform->rotation.euler.z )
        );
    }

    glm::qua qRotation { radiansRotation };
    modelMatrix *= glm::mat4_cast( qRotation );

    memcpy( mappedMemory, &modelMatrix, 4 * 4 * sizeof( float ) );
}

TransformLoader::~TransformLoader( ) {
    context->vma.unmapMemory( transformBuffer.second );
    context->vma.destroyBuffer( transformBuffer.first, transformBuffer.second );
}

std::pair< vk::Buffer, vma::Allocation > &TransformLoader::getBuffer( ) {
    return transformBuffer;
}

END_NAMESPACES