#pragma once

#include "../Core/Common.h"
#include "../ECS/ISystem.h"
#include "../ECS/CTransform.h"
#include "InstanceContext.h"

NAMESPACES( SomeVulkan, Graphics )

class STransformLoader {
private:
    void * mappedMemory;
    std::shared_ptr< InstanceContext > context;
    std::pair< vk::Buffer, vma::Allocation > transformBuffer;
public:
    explicit STransformLoader( std::shared_ptr< InstanceContext > context );
    std::pair< vk::Buffer, vma::Allocation >& getBuffer();
    void load( const std::shared_ptr< ECS::CTransform > &type );
    ~STransformLoader();
};

END_NAMESPACES

