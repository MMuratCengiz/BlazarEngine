#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CPointLight : public IComponent
{
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec3 specular;

    void fillFromPower( const uint32_t &power )
    {
        if ( power >= 7 )
        {
            attenuationLinear = 0.7;
            attenuationQuadratic = 1.8;
        }

        if ( power >= 13 )
        {
            attenuationLinear = 0.35;
            attenuationQuadratic = 0.44;
        }

        if ( power >= 20 )
        {
            attenuationLinear = 0.22;
            attenuationQuadratic = 0.20;
        }

        if ( power >= 32 )
        {
            attenuationLinear = 0.14;
            attenuationQuadratic = 0.07;
        }

        if ( power >= 50 )
        {
            attenuationLinear = 0.09;
            attenuationQuadratic = 0.032;
        }

        if ( power >= 65 )
        {
            attenuationLinear = 0.07;
            attenuationQuadratic = 0.017;
        }

        if ( power >= 100 )
        {
            attenuationLinear = 0.045;
            attenuationQuadratic = 0.0075;
        }

        if ( power >= 160 )
        {
            attenuationLinear = 0.027;
            attenuationQuadratic = 0.0028;
        }

        if ( power >= 200 )
        {
            attenuationLinear = 0.022;
            attenuationQuadratic = 0.0019;
        }

        if ( power >= 325 )
        {
            attenuationLinear = 0.014;
            attenuationQuadratic = 0.0007;
        }

        if ( power >= 600 )
        {
            attenuationLinear = 0.007;
            attenuationQuadratic = 0.0002;
        }

        if ( power >= 3250 )
        {
            attenuationLinear = 0.0014;
            attenuationQuadratic = 0.000007;
        }

        attenuationConstant = 1.0f;
    }
};

END_NAMESPACES