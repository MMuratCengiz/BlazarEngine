#include "FpsCamera.h"

NAMESPACES( ENGINE_NAMESPACE, Scene )

FpsCamera::FpsCamera( glm::vec3 position, glm::vec3 front ) {
    this->position = position;

    this->right = glm::vec3( 1.0f, 0.0f, 0.0f );
    this->yaw = -90.0f;
    this->pitch = 0.0f;
    this->worldUp = glm::vec3( 0.0f, 1.0f, 0.0f );
    this->front = glm::cross( right, worldUp );
    // todo dynamic

    this->projection = glm::perspective( glm::radians( 60.0f ), 800.0f / 600.0f, 0.1f, 100.0f );
    this->projection[ 1 ][ 1 ] = -this->projection[ 1 ][ 1 ];

    calculateView( );
}

void FpsCamera::updateAspectRatio( const uint32_t &windowWidth, const uint32_t &windowHeight ) {
    this->projection = glm::perspective( glm::radians( 60.0f ), windowWidth / ( float ) windowHeight, 0.1f, 100.0f );
    this->projection[ 1 ][ 1 ] *= -1.0f;
}

void FpsCamera::processKeyboardEvents( GLFWwindow *window ) {
    float sensitivity = ( Core::Time::getDeltaTime( ) );

    if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS ) {
        position -= front * glm::vec3( sensitivity, 0.0f, sensitivity );
    }

    if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS ) {
        position += front * glm::vec3( sensitivity, 0.0f, sensitivity );
    }

    if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS ) {
        position -= right * sensitivity;
    }

    if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS ) {
        position += right * sensitivity;
    }

    calculateView( );
}

void FpsCamera::processMouseEvents( GLFWwindow *window ) {
    double mouseX, mouseY;

    glfwGetCursorPos( window, &mouseX, &mouseY );

    if ( firstMouseMove ) {
        this->lastMouseX = mouseX;
        this->lastMouseY = mouseY;

        firstMouseMove = false;
    }

    float xOffset = mouseX - lastMouseX;
    float yOffset = lastMouseY - mouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    yaw += xOffset;
    pitch += yOffset;

    if ( pitch > 89.0f ) {
        pitch = 89.0f;
    } else if ( pitch < -89.0f ) {
        pitch = -89.0f;
    }

    glm::vec3 newFront;

    newFront.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
    newFront.y = sin( glm::radians( pitch ) );
    newFront.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
    this->front = glm::normalize( newFront );

    calculateView( );
}

void FpsCamera::calculateView( ) {
    right = glm::cross( front, worldUp );
    up = glm::cross( right, front );
    view = glm::lookAt( position, position + front, up );
}

glm::mat4 FpsCamera::getView( ) {
    return view;
}

glm::mat4 FpsCamera::getProjection( ) {
    return projection;
}

glm::vec3 FpsCamera::getPosition( ) {
    return position;
}

glm::vec3 FpsCamera::getFront( ) {
    return front;
}

END_NAMESPACES