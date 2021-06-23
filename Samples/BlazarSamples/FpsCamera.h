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

#include <BlazarECS/CCamera.h>
#include <BlazarCore/Time.h>
#include <BlazarInput/ActionMap.h>

namespace Sample
{

class FpsCamera
{
private:
    std::shared_ptr< BlazarEngine::ECS::CCamera > cameraComponent;

    glm::vec3 right { };
    glm::vec3 front { };
    glm::vec3 up { };

    glm::vec3 worldUp { };

    float pitch;
    float yaw;

    bool firstMouseMove { };

    float lastMouseY { };
    float lastMouseX { };

    void calculateView( );
public:
    explicit FpsCamera( std::shared_ptr< BlazarEngine::ECS::CCamera > cameraComponent, glm::vec3 front = glm::vec3( 0.0f, 0.0f, 1.0f ) );
    void processKeyboardEvents( GLFWwindow *window );
    void processMouseEvents( GLFWwindow *window );
    void updateAspectRatio( const uint32_t &windowWidth, const uint32_t &windowHeight );
    glm::vec3 getFront( );
};

}