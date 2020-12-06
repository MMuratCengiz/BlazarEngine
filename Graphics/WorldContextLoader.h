#pragma once

#include "../Core/Common.h"
#include "../ECS/ISystem.h"
#include "../ECS/CTransform.h"
#include "InstanceContext.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct WorldContext {
    alignas(16) glm::vec4 worldPosition;
};

class WorldContextLoader {
private:
    void * mappedMemory;
    std::shared_ptr< InstanceContext > context;
    std::pair< vk::Buffer, vma::Allocation > buffer;
    WorldContext worldContext{ };
public:
    explicit WorldContextLoader( std::shared_ptr< InstanceContext > context );
    WorldContext& getWorldContext();
    void update( );
    std::pair< vk::Buffer, vma::Allocation >& getBuffer();
    ~WorldContextLoader();
};

END_NAMESPACES


