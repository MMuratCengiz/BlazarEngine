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

```
git submodule update --init --recursive
.\external\vcpkg\scripts\bootstrap.sh 
.\external\vcpkg\vcpkg.exe install glfw3:x64-windows-static
.\external\vcpkg\vcpkg.exe install Vulkan:x64-windows-static
.\external\vcpkg\vcpkg.exe install glslang:x64-windows-static
.\external\vcpkg\vcpkg.exe install bullet3:x64-windows-static
# or for unix:
.\external\vcpkg\vcpkg.exe install glfw3
.\external\vcpkg\vcpkg.exe install bullet3
.\external\vcpkg\vcpkg.exe install Vulkan
.\external\vcpkg\vcpkg.exe install glslang
```

If you're not using the presets, make sure to add the cacheVariables as cmake options.

To use BlazarEngine as an ExternalProject:

```

```