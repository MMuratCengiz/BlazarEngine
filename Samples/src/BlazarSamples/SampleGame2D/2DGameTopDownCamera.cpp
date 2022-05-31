#include <BlazarSamples/SampleGame2D/2DGameTopDownCamera.h>

using namespace BlazarEngine;

TDGameTopDownCamera::TDGameTopDownCamera( Scene::World *world ) : world( world )
{
    auto cameraComponent = createComponent< ECS::CCamera >( );

    float w = 1920.0f;
    float h = 1080.0f;

    this->left = 0.0f;
    this->right = w;
    this->top = 0.0f;
    this->bottom = h;

    this->zoomX = ( right - left ) / 100;
    this->zoomY = ( bottom - top ) / 100;

    cameraComponent->projection = glm::ortho( left, right, bottom, top, -1.0f, 1.0f );
    cameraComponent->position = glm::vec3( w / 2, h / 2, 1.0f );
    cameraComponent->view = glm::mat4 { 1 };
    glm::rotate( cameraComponent->view, glm::radians( 90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
    glm::translate( cameraComponent->view, glm::vec3( 0.0f, 0.0f, -1.0f ) );

    world->getActionMap( )->registerScroll(
            [ & ]( double x, double y )
            {
                float speed = 10000 * Core::Time::getDeltaTime( );
                float scaledZoomX = y * zoomX * speed;
                float scaledZoomY = y * zoomY * speed;

                left += scaledZoomX;
                right -= scaledZoomX;
                top += scaledZoomY;
                bottom -= scaledZoomY;

                getComponent< ECS::CCamera >( )->projection = glm::ortho( left, right, bottom, top, -1.0f, 1.0f );
            } );

    world->getActionMap( )->registerAction( "Pan", { Input::MouseKeyCode::Button2 } );
    world->getActionMap( )->subscribeToAction( "Pan",
                                               [ & ]( const std::string &actionName, const Input::KeyState &pressForm, float _ )
                                               {
                                                   this->panning = pressForm == Input::KeyState::Pressed;
                                               } );

    world->getActionMap( )->registerMouseMove(
            [ & ]( double x, double y )
            {
                if ( prevX == 0.0f )
                {
                    prevX = x;
                }

                if ( prevY == 0.0f )
                {
                    prevY = y;
                }

                if ( panning )
                {
                    auto speedX = ( prevX - x ) * 2000 * Core::Time::getDeltaTime( );
                    auto speedY = ( prevY - y ) * 2000 * Core::Time::getDeltaTime( );
                    left -= speedX;
                    right -= speedX;
                    top += speedY;
                    bottom += speedY;

                    getComponent< ECS::CCamera >( )->projection = glm::ortho( left, right, bottom, top, -1.0f, 1.0f );
                }

                prevX = x;
                prevY = y;
            } );
}

