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
    explicit FpsCamera( std::shared_ptr< BlazarEngine::ECS::CCamera > cameraComponent, glm::vec3 front = glm::vec3( 0.0f, 0.0f, -1.0f ) );
    void processKeyboardEvents( GLFWwindow *window );
    void processMouseEvents( GLFWwindow *window );
    void updateAspectRatio( const uint32_t &windowWidth, const uint32_t &windowHeight );
    glm::vec3 getFront( );
};

}