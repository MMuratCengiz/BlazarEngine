#pragma once

#include "../core/Common.h"
#include "IComponent.h"
#include "../graphics/DrawDescription.h"

NAMESPACES( SomeVulkan, ECS )

class Renderable : public IComponent {
private:
    Graphics::DrawDescription drawDescription;

public:
    static const uint32_t UID;

    uint32_t getId( ) override;

    void setDrawDescription( Graphics::DrawDescription& vd );

    [[nodiscard]] const Graphics::DrawDescription& getDrawDescription( ) const;
};


#define RENDERABLE auto * renderable = new SomeVulkan::ECS::Renderable( );                     \
                   vComponents.emplace_back( std::shared_ptr< SomeVulkan::ECS::IComponent > (  \
                   reinterpret_cast< SomeVulkan::ECS::IComponent* > ( renderable ) ) );

END_NAMESPACES