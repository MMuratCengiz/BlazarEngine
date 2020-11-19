#pragma once

#include "../core/Common.h"
#include "IComponent.h"
#include "ECSUtilities.h"
#include "../graphics/DrawDescription.h"

NAMESPACES( SomeVulkan, ECS )

struct Renderable : public IComponent {
private:
    Graphics::DrawDescription drawDescription;
public:
    [[nodiscard]] const Graphics::DrawDescription& getDrawDescription( ) {
        return drawDescription;
    }

    inline void setDrawDescription( Graphics::DrawDescription& vd ) {
        drawDescription = std::move( vd );
    }
};

END_NAMESPACES