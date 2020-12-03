#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

namespace Material {
	enum class Filter {
		Nearest,
		Linear,
		CubicIMG,
		CubicEXT
	};

	enum class AddressMode {
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge,
		MirrorClampToEdgeKHR
	};

	enum class MipmapMode {
		eNearest,
		eLinear
	};

	struct TextureInfo {
		std::string path;
		Filter magFilter = Filter::Linear;
		Filter minFilter = Filter::Linear;
		AddressMode U = AddressMode::Repeat;
		AddressMode V = AddressMode::Repeat;;
		AddressMode W = AddressMode::Repeat;;
		MipmapMode mipmapMode = MipmapMode::eLinear;
		float mipLodBias = 0.0f;
		float minLod = 0.0f;
		float maxLod = 0.0f;
	};
};

struct CMaterial : public IComponent {
	std::vector< Material::TextureInfo > textures;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
};

END_NAMESPACES