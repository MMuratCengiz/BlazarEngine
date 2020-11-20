#pragma once

#include "Camera.h"

NAMESPACES( SomeVulkan, Scene )

class FpsCamera : public Camera {
private:
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 target;
    glm::vec3 up;

    glm::vec3 worldUp;

    GLfloat pitch;
    GLfloat yaw;

    bool firstMouseMove;

    GLfloat lastMouseY;
    GLfloat lastMouseX;

    void calculateView();
public:
    FpsCamera( glm::vec3 position, glm::vec3 target );

    void processKeyboardEvents( GLFWwindow* window, float deltaTime );
    void processMouseEvents( GLFWwindow* window, double mouseX, double mouseY, float deltaTime );
    glm::mat4 getView() override;
    glm::mat4 getProjection() override;
    glm::vec3 getPosition() override;
    glm::vec3 getFront();
};

END_NAMESPACES