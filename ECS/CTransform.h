#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

enum class RotationUnit {
    Radians,
    Degrees
};

struct Rotation {
    RotationUnit rotationUnit { RotationUnit::Degrees };
    glm::vec3 euler { 0.0f };
};

struct CTransform : public IComponent {
    glm::vec3 position { 0.0f };
    glm::vec3 scale { 1.0f };
    Rotation rotation { };
};


END_NAMESPACES