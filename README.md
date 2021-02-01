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


### The Engine itself currently also integrates the Bullet3 Physics engine.


## Todo:

- Animations.
- Physically based rendering.
- Scripting integration.


# Demo

![Demo](/docs/sample_1.mp4)
