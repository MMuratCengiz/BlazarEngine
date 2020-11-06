//
// Created by Murat on 10/24/2020.
//

#include "Renderable.h"


NAMESPACES( SomeVulkan, ECS )

const uint32_t Renderable::UID = 1;

const Graphics::DrawDescription& Renderable::getDrawDescription( ) const {
    return drawDescription;
}

void Renderable::setDrawDescription( Graphics::DrawDescription& vd ) {
    drawDescription = std::move( vd );
}

uint32_t Renderable::getId( ) {
    return UID;
}

END_NAMESPACES