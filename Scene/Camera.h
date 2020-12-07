#pragma once

#include "../Core/Common.h"

NAMESPACES( ENGINE_NAMESPACE, Scene )

class Camera
{
public:
    virtual glm::mat4 getView( ) = 0;
    virtual glm::mat4 getProjection( ) = 0;
    virtual glm::vec3 getPosition( ) = 0;

    virtual void processKeyboardEvents( GLFWwindow *window ) = 0;
    virtual void processMouseEvents( GLFWwindow *window ) = 0;
};

END_NAMESPACES