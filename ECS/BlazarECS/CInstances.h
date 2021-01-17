#pragma once

#include <BlazarCore/Common.h>
#include "CTransform.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CInstances : public IComponent
{
public:
    std::vector< std::shared_ptr< CTransform > > transforms;

    BLAZAR_COMPONENT( CInstances )
};


END_NAMESPACES