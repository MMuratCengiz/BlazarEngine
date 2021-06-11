#include "FpsCamera.h"
#include <vector>

namespace Sample
{

FpsCamera::FpsCamera( std::shared_ptr< BlazarEngine::ECS::CCamera > cameraComponent, glm::vec3 front )
{
    this->cameraComponent = std::move( cameraComponent );
    this->right = glm::vec3( 1.0f, 0.0f, 0.0f );
    this->yaw = -90.0f;
    this->pitch = 0.0f;
    this->worldUp = glm::vec3( 0.0f, 1.0f, 0.0f );
    this->front = glm::cross( right, worldUp );
    // todo dynamic

    this->cameraComponent->projection = glm::perspective( glm::radians( 60.0f ), 800.0f / 600.0f, 0.1f, 100.0f );
    this->cameraComponent->projection = VK_CORRECTION_MATRIX * this->cameraComponent->projection;

    calculateView( );
}


void FpsCamera::updateAspectRatio( const uint32_t &windowWidth, const uint32_t &windowHeight )
{
    this->cameraComponent->projection = glm::perspective( glm::radians( 60.0f ), windowWidth / ( float ) windowHeight, 0.1f, 100.0f );
    this->cameraComponent->projection = VK_CORRECTION_MATRIX * this->cameraComponent->projection;
}

void FpsCamera::processKeyboardEvents( GLFWwindow *window )
{
    float sensitivity = ( BlazarEngine::Core::Time::getDeltaTime( ) ) * 10.0f;

    if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
    {
        this->cameraComponent->position -= front * glm::vec3( sensitivity, 0.0f, sensitivity );
    }

    if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
    {
        this->cameraComponent->position += front * glm::vec3( sensitivity, 0.0f, sensitivity );
    }

    if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
    {
        this->cameraComponent->position -= right * sensitivity;
    }

    if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
    {
        this->cameraComponent->position += right * sensitivity;
    }

    if ( glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS )
    {
        this->cameraComponent->position += up * sensitivity;
    }

    if ( glfwGetKey( window, GLFW_KEY_F ) == GLFW_PRESS )
    {
        this->cameraComponent->position -= up * sensitivity;
    }

    if ( glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS )
    {
        FILE *file = fopen( "C:/Users/Murat/Documents/Garbage/debugCam.txt", "ab+" );

        std::stringstream contents;
        contents << "position: " <<
                 this->cameraComponent->position.x << "," <<
                 this->cameraComponent->position.y << "," <<
                 this->cameraComponent->position.z << "\r\n";

        contents << "front: " <<
                 this->front.x << "," <<
                 this->front.y << "," <<
                 this->front.z << "\r\n";


        fwrite( contents.str( ).data( ), 1, contents.str( ).size( ), file );

        fclose( file );
    }

    calculateView( );
}

void FpsCamera::processMouseEvents( GLFWwindow *window )
{
    double mouseX, mouseY;

    glfwGetCursorPos( window, &mouseX, &mouseY );

    if ( firstMouseMove )
    {
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

    if ( pitch > 89.0f )
    {
        pitch = 89.0f;
    }
    else if ( pitch < -89.0f )
    {
        pitch = -89.0f;
    }

    glm::vec3 newFront;

    newFront.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
    newFront.y = sin( glm::radians( pitch ) );
    newFront.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
    this->front = glm::normalize( newFront );

    calculateView( );
}

void FpsCamera::calculateView( )
{
    right = glm::cross( front, worldUp );
    up = glm::cross( right, front );
    this->cameraComponent->view = glm::lookAt( this->cameraComponent->position, this->cameraComponent->position + front, up );
}

glm::vec3 FpsCamera::getFront( )
{
    return front;
}

}