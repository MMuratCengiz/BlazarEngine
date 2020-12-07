#pragma once

#include "../Core/Common.h"
#include "IComponent.h"
#include "ECSUtilities.h"
#include "../Graphics/DrawDescription.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct Renderable : public IComponent
{
private:
    Graphics::DrawDescription drawDescription;
public:
    [[nodiscard]] const Graphics::DrawDescription &getDrawDescription( )
    {
        return drawDescription;
    }

    inline void setDrawDescription( Graphics::DrawDescription &vd )
    {
        drawDescription = std::move( vd );
    }
};

END_NAMESPACES