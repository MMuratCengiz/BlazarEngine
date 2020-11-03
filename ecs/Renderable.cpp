//
// Created by Murat on 10/24/2020.
//

#include "Renderable.h"

#include <utility>

using namespace SomeVulkan::ECS;

const uint32_t Renderable::UID = 1;

const SomeVulkan::Graphics::DrawDescription& SomeVulkan::ECS::Renderable::getDrawDescription( ) const {
    return drawDescription;
}

void SomeVulkan::ECS::Renderable::setDrawDescription( SomeVulkan::Graphics::DrawDescription& vd ) {
    drawDescription = std::move( vd );
}

uint32_t Renderable::getId( ) {
    return UID;
}
