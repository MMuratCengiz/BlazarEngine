#pragma once

#include "../Core/Common.h"
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
    std::string path;
    CullMode cullMode = CullMode::BackFace;
};

END_NAMESPACES