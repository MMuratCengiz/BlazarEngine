#pragma once

#include "../core/Common.h"
#include "IComponent.h"
#include "../graphics/VertexDescriptor.h"

NAMESPACES( SomeVulkan, ECS )

class Renderable : public IComponent {
private:
    Graphics::VertexDescriptor vertexDescriptor;

public:
    static const uint32_t UID;

    uint32_t getId( ) override;

    void setVertexDescriptor( Graphics::VertexDescriptor vd );

    Graphics::VertexDescriptor getVertexDescriptor( );
};


#define RENDERABLE auto * renderable = new SomeVulkan::ECS::Renderable( );                     \
                   vComponents.emplace_back( std::shared_ptr< SomeVulkan::ECS::IComponent > (  \
                   reinterpret_cast< SomeVulkan::ECS::IComponent* > ( renderable ) ) );

END_NAMESPACES