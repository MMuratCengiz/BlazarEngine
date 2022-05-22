#include "2DGameTopDownCamera.h"

using namespace BlazarEngine;

TDGameTopDownCamera::TDGameTopDownCamera( )
{
    auto cameraComponent = createComponent< ECS::CCamera >( );

    float w = 1920.0f;
    float h = 1080.0f;

    cameraComponent->position = glm::vec3(w / 2, h / 2, 1.0f);
    cameraComponent->position = glm::vec3(0, 0, 1.0f);
    cameraComponent->projection = glm::ortho( 0.0f, w, h, 0.0f, -1.0f, 1.0f );
    cameraComponent->view = glm::mat4 { 1 };
    glm::rotate( cameraComponent->view, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f) );
    glm::translate(cameraComponent->view, glm::vec3( 0.0f, 0.0f, -1.0f ) );
}

