#pragma once

#include "../core/common.h"

#include <fstream>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_parser.hpp>

/*
TODO List:
- Support matrix as vertex input.
*/
NAMESPACES( SomeVulkan, Graphics )

typedef struct MVP {
	glm::mat4x4 model;
	glm::mat4x4 view;
	glm::mat4x4 projection;

	[[nodiscard]] uint32_t size( ) const {
		return 3 /*Matrices*/ * 4 /*columns*/ * 4 /*rows*/;
	}

	static uint32_t fullSize( ) {
		return 3 /*Matrices*/ * 4 /*columns*/ * 4 /*rows*/ * sizeof( float );
	}

	[[nodiscard]] const MVP* data( ) const {
		return this;
	}
} MVP;

struct DescriptorSetBinding {
	uint16_t index;
	vk::DescriptorType type;
	vk::DescriptorSetLayoutBinding binding;
	vk::DeviceSize size;
	std::string name;
};

struct DescriptorSet {
	uint32_t id;
	std::vector< vk::DescriptorSetLayoutBinding > descriptorSetLayoutBindings;
	std::vector< DescriptorSetBinding > descriptorSetBindings;
	std::unordered_map< std::string, DescriptorSetBinding > descriptorSetBindingMap;
};

struct ShaderInfo {
	vk::ShaderStageFlagBits type;
	std::string path;
};

class GLSLShaderSet {
private:
	struct SpvDecoration {
		spirv_cross::SPIRType type;
		uint32_t set;
		uint32_t location;
		uint32_t binding;
		uint32_t arraySize;
		uint32_t size;
		std::string name;
	};

	struct GLSLType {
		vk::Format format;
		uint32_t size;
	};

	struct DescriptorBindingCreateInfo {
		uint32_t binding;
		spirv_cross::Resource resource;
		vk::DescriptorType type;
		vk::ShaderStageFlagBits stage;
	};

	std::vector< vk::VertexInputBindingDescription > inputBindingDescriptions;
	std::vector< vk::VertexInputAttributeDescription > vertexAttributeDescriptions;
	std::unordered_map< uint32_t, DescriptorSet > descriptorSetMap;
	std::vector< DescriptorSet > descriptorSets;

	bool interleavedMode;
public:
    explicit GLSLShaderSet( const std::vector< ShaderInfo >& shaders, const bool& interleavedMode = true );
	
	inline const std::vector< DescriptorSet >& getDescriptorSets( ) {
		return descriptorSets;
	}
	
	inline const DescriptorSet& getDescriptorSetBySetId( uint32_t id ) {
		return descriptorSetMap[ id ];
	}
	
	inline const std::vector< vk::VertexInputBindingDescription >& getInputBindingDescriptions( ) {
		return inputBindingDescriptions;
	}

	inline const std::vector< vk::VertexInputAttributeDescription >& getVertexAttributeDescriptions( ) {
		return vertexAttributeDescriptions;
	}
private:
	void onEachShader( const ShaderInfo& shaderInfo );
    static std::vector< uint32_t > readFile( const std::string &filename );
	void ensureSetExists( uint32_t set );
	void createVertexInput( const uint32_t& offset, const GLSLType& type, const uint32_t& location );

	void createDescriptorSetBinding( const spirv_cross::Compiler& compiler, const DescriptorBindingCreateInfo& bindingCreateInfo );
	GLSLType spvToGLSLType( const spirv_cross::SPIRType& type );
	SpvDecoration getDecoration( const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource );
};


END_NAMESPACES
