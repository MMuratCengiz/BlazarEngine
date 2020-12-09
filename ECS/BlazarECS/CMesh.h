#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

enum class CullMode
{
    FrontAndBackFace,
    BackFace,
    FrontFace,
    None
};

struct CMesh : public IComponent
{
public:
    std::string path;
    CullMode cullMode = CullMode::BackFace;
    BLAZAR_COMPONENT( CMesh )
};

END_NAMESPACES