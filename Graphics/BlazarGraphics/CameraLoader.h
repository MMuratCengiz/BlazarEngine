#pragma once

#include <BlazarECS/ECS.h>
#include "InstanceContext.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class CameraLoader
{
private:
    struct VP
    {
        glm::mat4 view;
        glm::mat4 projection;
    };

    void *mappedMemory;
    std::shared_ptr< InstanceContext > context;
    std::pair< vk::Buffer, vma::Allocation > cameraBuffer;
    std::shared_ptr< ECS::CCamera > camera{};
public:
    explicit CameraLoader( std::shared_ptr< InstanceContext > context );
    std::pair< vk::Buffer, vma::Allocation > &getBuffer( );
    void reload( std::shared_ptr< ECS::CCamera > pCamera );
    void reload( );
    ~CameraLoader( );
};

END_NAMESPACES

