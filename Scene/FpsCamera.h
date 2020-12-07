#pragma once

#include "Camera.h"
#include "../Core/Time.h"

NAMESPACES( ENGINE_NAMESPACE, Scene )

class FpsCamera : public Camera
{
private:
    glm::mat4 view { };
    glm::mat4 projection { };

    glm::vec3 position { };
    glm::vec3 right { };
    glm::vec3 front { };
    glm::vec3 up { };

    glm::vec3 worldUp { };

    GLfloat pitch;
    GLfloat yaw;

    bool firstMouseMove { };

    GLfloat lastMouseY { };
    GLfloat lastMouseX { };

    void calculateView( );
public:
    explicit FpsCamera( glm::vec3 position, glm::vec3 front = glm::vec3( 0.0f, 0.0f, -1.0f ) );
    void updateAspectRatio( const uint32_t &windowWidth, const uint32_t &windowHeight );
    void processKeyboardEvents( GLFWwindow *window ) override;
    void processMouseEvents( GLFWwindow *window ) override;
    glm::mat4 getView( ) override;
    glm::mat4 getProjection( ) override;
    glm::vec3 getPosition( ) override;
    glm::vec3 getFront( );
};

END_NAMESPACES