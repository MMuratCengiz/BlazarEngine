#pragma once

#include "../core/Common.h"
#include "../ECS.h"
#include "../ecs/Renderable.h"

NAMESPACES( SomeVulkan, RenderObject )

using namespace ECS;

class Triangle2D : public SomeVulkan::ECS::IGameEntity {
    START_COMPONENTS
        RENDERABLE
    END_COMPONENTS

    Triangle2D( ) {
        getComponent< Renderable >( )->setVertexDescriptor( Graphics::VertexDescriptor {
                .indexedMode = true,
                .vertices = {
                        -0.5f, -0.5f, 3.0f, 1.0f, 0.0f, 0.0f,
                         0.5f, -0.5f, 3.0f, 0.0f, 1.0f, 0.0f,
                         0.5f,  0.5f, 3.0f, 0.0f, 0.0f, 1.0f,
                        -0.5f,  0.5f, 3.0f, 1.0f, 1.0f, 1.0f
                },
                .indices = {
                    0, 1, 2, 2, 3, 0
                }
        } );
    }
};

END_NAMESPACES