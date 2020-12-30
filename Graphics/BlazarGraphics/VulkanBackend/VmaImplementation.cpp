#define VMA_IMPLEMENTATION

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"