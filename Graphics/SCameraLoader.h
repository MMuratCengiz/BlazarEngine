#pragma once

#include "../Core/Common.h"
#include "../ECS/ISystem.h"
#include "../ECS/CTransform.h"
#include "InstanceContext.h"
#include "../Scene/Camera.h"

NAMESPACES( SomeVulkan, Graphics )

class SCameraLoader {
private:
    struct VP {
        glm::mat4 view;
        glm::mat4 projection;
    };

    void * mappedMemory;
    std::shared_ptr< InstanceContext > context;
    std::pair< vk::Buffer, vma::Allocation > cameraBuffer;
    std::shared_ptr< Scene::Camera > camera;
public:
    explicit SCameraLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< Scene::Camera > camera );
    std::pair< vk::Buffer, vma::Allocation >& getBuffer();
    void reload( );
    ~SCameraLoader();
};

END_NAMESPACES

