#pragma once

#include "../Core/Common.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

enum class ConstantName
{
    ShaderInputViewProjection,
    ShaderInputWorldContext,
    ShaderInputEnvironmentLights,
    ShaderInputMaterial,
    ShaderInputSampler
};

class Constants
{
public:
    static inline std::string getConstant( ConstantName constantName )
    {
        switch ( constantName )
        {
            case ConstantName::ShaderInputViewProjection:
                return "ViewProjection";
            case ConstantName::ShaderInputWorldContext:
                return "WorldContext";
            case ConstantName::ShaderInputEnvironmentLights:
                return "EnvironmentLights";
            case ConstantName::ShaderInputMaterial:
                return "Material";
            case ConstantName::ShaderInputSampler:
                return "Texture";
        }

        return "";
    }
};

END_NAMESPACES
