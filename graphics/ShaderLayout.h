#pragma once

#include "../core/Common.h"

NAMESPACES(SomeVulkan, Graphics)

typedef enum class DataType_T {
    Float,
    Double,
    Boolean,
    Short,
    Int,
    Long,
    Vec2,
    Vec3,
    Vec4,
} DataType;

typedef struct ShaderAttribute_T {
    DataType dataType;
    void * data;
} ShaderAttribute;

class ShaderLayout {
protected:
    std::vector< ShaderAttribute > attributes;

    virtual void initializeAttributes() = 0;
public:
    [[nodiscard]] std::vector< ShaderAttribute > getAttributes() const {
        return attributes;
    }
};

END_NAMESPACES