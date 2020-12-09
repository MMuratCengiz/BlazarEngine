#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

enum class RotationUnit
{
    Radians,
    Degrees
};

struct Rotation
{
    RotationUnit rotationUnit { RotationUnit::Degrees };
    glm::vec3 euler { 0.0f };
};

struct CTransform : public IComponent
{
public:
    glm::vec3 position { 0.0f };
    glm::vec3 scale { 1.0f };
    Rotation rotation { };

    BLAZAR_COMPONENT( CTransform )
};


END_NAMESPACES