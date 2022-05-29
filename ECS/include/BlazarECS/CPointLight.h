/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CPointLight : public IComponent
{
public:
    float attenuationConstant { };
    float attenuationLinear { };
    float attenuationQuadratic { };
    glm::vec3 position { };
    glm::vec3 diffuse { };
    glm::vec3 specular { };

    BLAZAR_COMPONENT( CPointLight )

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