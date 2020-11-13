#pragma once

#include "../core/Common.h"
#include "../graphics/ShaderLayout.h"
#include <glm/glm.hpp>

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

class DefaultShaderLayout : public ShaderLayout {
private:
	const uint32_t ALL_ELEMENTS_COUNT = 5;

	vk::VertexInputAttributeDescription v1Desc = {
		/*location = */0,
		/*binding =  */0,
		/*format =   */vk::Format::eR32G32B32Sfloat,
		/*offset =   */0
	};

	vk::VertexInputAttributeDescription v2Desc = {
		/*location = */1,
		/*binding =  */0,
		/*format =   */vk::Format::eR32G32Sfloat,
		/*offset =   */3 * sizeof( float )
	};

	vk::DescriptorSetLayoutBinding uniformDesc{
		/*binding           =*/0,
		/*descriptorType    =*/vk::DescriptorType::eUniformBuffer,
		/*descriptorCount   =*/1,
		/*stageFlags        =*/vk::ShaderStageFlagBits::eVertex
	};

	vk::DescriptorSetLayoutBinding textureDesc{
		/*binding           =*/1,
		/*descriptorType    =*/vk::DescriptorType::eCombinedImageSampler,
		/*descriptorCount   =*/1,
		/*stageFlags        =*/vk::ShaderStageFlagBits::eFragment
	};

public:
	DefaultShaderLayout( ) {
		inputBindingDescription = {
			/*binding    = */0,
			/*stride     = */5 * sizeof( float ),
			/*inputRate  = */vk::VertexInputRate::eVertex,
		};

		vertexAttributeDescriptions = {
				v1Desc, v2Desc
		};

		descriptorSetBindings.resize( 2 );
		descriptorSetBindings[ 0 ].index = 0;
		descriptorSetBindings[ 0 ].type = vk::DescriptorType::eUniformBuffer;
		descriptorSetBindings[ 0 ].binding = uniformDesc;
		descriptorSetBindings[ 0 ].size = MVP::fullSize( );

		descriptorSetBindings[ 1 ].index = 1;
		descriptorSetBindings[ 1 ].type = vk::DescriptorType::eCombinedImageSampler;
		descriptorSetBindings[ 1 ].binding = textureDesc;
		
	};
};

END_NAMESPACES