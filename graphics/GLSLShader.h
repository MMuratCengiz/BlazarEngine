#pragma once

#include "../core/common.h"

#include <fstream>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_parser.hpp>

NAMESPACES( SomeVulkan, Graphics )

enum class ShaderType2 {
    Vertex,
    Fragment
};

struct DescriptorSetBinding2 {
	uint16_t index;
	vk::DescriptorType type;
	vk::DescriptorSetLayoutBinding binding;
	vk::DeviceSize size;
};

class GLSLShader {
private:
	ShaderType2 type;

	vk::VertexInputBindingDescription inputBindingDescription;

    std::vector< vk::VertexInputAttributeDescription > vertexAttributeDescriptions;
    std::vector< vk::DescriptorSetLayoutBinding > descriptorSetLayoutBindings;
	std::vector< DescriptorSetBinding2 > descriptorSetBindings;
public:
    GLSLShader( ShaderType2 type, const std::string& path );
private:
    static std::vector< uint32_t > readFile( const std::string &filename );
};

END_NAMESPACES
