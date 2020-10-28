//
// Created by Murat on 10/24/2020.
//

#include "Renderable.h"

#include <utility>

using namespace SomeVulkan::ECS;

const uint32_t Renderable::UID = 1;

SomeVulkan::Graphics::VertexDescriptor SomeVulkan::ECS::Renderable::getVertexDescriptor( ) {
    return vertexDescriptor;
}

void SomeVulkan::ECS::Renderable::setVertexDescriptor( SomeVulkan::Graphics::VertexDescriptor vd ) {
    vertexDescriptor = std::move( vd );
}

uint32_t Renderable::getId( ) {
    return UID;
}
