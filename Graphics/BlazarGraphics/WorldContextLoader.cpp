#include "WorldContextLoader.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

WorldContextLoader::WorldContextLoader( std::shared_ptr< InstanceContext > context ) : context( std::move( context ) )
{
    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    bufferCreateInfo.size = sizeof( WorldContext );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo;
    allocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    allocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal;

    buffer = this->context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    mappedMemory = this->context->vma.mapMemory( buffer.second );
}

WorldContext &WorldContextLoader::getWorldContext( )
{
    return worldContext;
}

void WorldContextLoader::update( )
{
    memcpy( mappedMemory, &worldContext, sizeof( WorldContext ) );
}

WorldContextLoader::~WorldContextLoader( )
{
    context->vma.unmapMemory( buffer.second );
    context->vma.destroyBuffer( buffer.first, buffer.second );
}

std::pair< vk::Buffer, vma::Allocation > &WorldContextLoader::getBuffer( )
{
    return buffer;
}

END_NAMESPACES