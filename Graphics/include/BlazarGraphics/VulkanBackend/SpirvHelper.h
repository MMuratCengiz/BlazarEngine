#pragma once

#include <BlazarCore/Common.h>
#include "../GraphicsCommonIncludes.h"
#include <glslang/SPIRV/GlslangToSpv.h>

NAMESPACES(ENGINE_NAMESPACE, Graphics)

class SpirvHelper
{
public:
    static void init();
    static void destroy();
    static void initResources(TBuiltInResource &Resources);
    static EShLanguage findLanguage(const vk::ShaderStageFlagBits shader_type);
    static std::vector< uint32_t > GLSLtoSPV(vk::ShaderStageFlagBits shader_type, const char *pShader );
};

END_NAMESPACES