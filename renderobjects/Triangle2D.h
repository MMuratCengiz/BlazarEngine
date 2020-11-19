#pragma once

#include "../core/Common.h"
#include "../ECS.h"
#include "../ecs/Renderable.h"
#include "../graphics/CommonTextures.h"

NAMESPACES( SomeVulkan, RenderObject )

using namespace ECS;
using namespace Graphics;

class Triangle2D : public SomeVulkan::ECS::IGameEntity {
    Graphics::DrawDescription drawDescription { };
    std::shared_ptr< TextureLoader > tex;

    Triangle2D( ) {
        auto renderable = createComponent< ECS::Renderable >( );

        tex = std::make_shared< TextureLoader >( 2, "/assets/textures/container.jpg" );

        drawDescription.indexedMode = true;

        Core::DynamicMemory &memory = drawDescription.vertexMemory;
        // 3x Vertices, 2x TextureCoordinates
        memory.attachElements< float >( { -1.0f, -1.0f, 0.0f } ); memory.attachElements< float >( { 1.0f, 0.0f } );
        memory.attachElements< float >( {  1.0f, -1.0f, 0.0f } ); memory.attachElements< float >( { 0.0f, 0.0f } );
        memory.attachElements< float >( {  1.0f,  1.0f, 0.0f } ); memory.attachElements< float >( { 0.0f, 1.0f } );
        memory.attachElements< float >( { -1.0f,  1.0f, 0.0f } ); memory.attachElements< float >( { 1.0f, 1.0f } );

        drawDescription.indices = {
                0, 1, 2, 2, 3, 0
        };

        drawDescription.textures.emplace_back( tex );

        renderable->setDrawDescription( drawDescription );
    };

};

END_NAMESPACES