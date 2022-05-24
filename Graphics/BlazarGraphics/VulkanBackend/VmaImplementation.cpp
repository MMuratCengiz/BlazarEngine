/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define VMA_IMPLEMENTATION

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VMA_STATS_STRING_ENABLED 0
#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include <tiny_gltf.h>