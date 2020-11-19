#pragma once

#include "../core/Common.h"
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
		Filter magFilter;
		Filter minFilter;
		AddressMode U;
		AddressMode V;
		AddressMode W;
		MipmapMode mipmapMode;
		float mipLodBias;
		float minLod;
		float maxLod;
	};
};

struct CMaterial : public IComponent {
	std::vector< Material::TextureInfo > textures;
};

END_NAMESPACES