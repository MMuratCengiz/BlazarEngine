#pragma once

#include "InstanceContext.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class TransformLoader
{
private:
    void *mappedMemory;
    std::shared_ptr< InstanceContext > context;
    std::pair< vk::Buffer, vma::Allocation > transformBuffer;
public:
    explicit TransformLoader( std::shared_ptr< InstanceContext > context );
    std::pair< vk::Buffer, vma::Allocation > &getBuffer( );
    void load( const std::shared_ptr< ECS::CTransform > &type );
    ~TransformLoader( );
    static glm::mat4 getModelMatrix( const std::shared_ptr< ECS::CTransform > &transform );
};

END_NAMESPACES

