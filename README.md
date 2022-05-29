# Blazar Engine

Custom Game Engine developed in C++ and Vulkan, it is still work in progress, the design is based off of:

- Pure Entity Component System design, everything is an entity(containing only data). Logic is then handled through Systems. 
- A RenderGraph system for the graphics backend.
  

## The graphics engine supports:

- The expected core of, model loading, texture loading and cameras.
- ShadowMaps, ie. rendering shadows.
- Instanced Rendering.
- Tessellation and Height maps.
- Directional/Ambient/Spot lights.
- SMAA.
- Animations


### The Engine itself currently also integrates the Bullet3 Physics engine.


## Todo:

- Physically based rendering.
- Scripting integration.


# Demo

Note the visual quality and framerate are compressed so the gif could be embedded in a reasonable file size and browsing speed. 

![Demo](/docs/sample_1.gif)


# Building:

You may need install the Vulkan SDK and set the VULKAN_SDK environment variable beforehand.

In OSX, Molten SDK variables should also be set, looks something like;
```
VULKAN_SDK=/Users/{your-user}/VulkanSDK/1.3.211.0/macOS
DYLD_LIBRARY_PATH=${VULKAN_SDK}/lib:${DYLD_LIBRARY_PATH}:
VK_LAYER_PATH=${VULKAN_SDK/share/vulkan/explicit_layer.d}
VK_ICD_FILENAMES=${VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json
```

Initial setup:
``` 
git submodule add https://github.com/MMuratCengiz/BlazarEngine.git
git submodule update --init --recursive
{PathToBlazarEngine}.\external\vcpkg\scripts\bootstrap.sh 
```

Then to include it in your project:

```
{PathToBlazarEngine} would be something like external/BlazarEngine depending where you added it as a submodule
# Currently this is required in Win32 to correctly link to Bullet libraries
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/{PathToBlazarEngine}/cmake/include_definitions.cmake)
SET(BLAZAR_INSTALL_LIBS ON)
SET(BLAZAR_BUILD_AS_LIB ON)
ADD_SUBDIRECTORY({PathToBlazarEngine}) # Path to BlazarEngine

ADD_EXECUTABLE(YourGame main.cpp)

TARGET_LINK_LIBRARIES(YourGame PRIVATE BlazarEngine)
```