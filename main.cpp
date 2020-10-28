
#include <iostream>
#include "core/Game.h"
#include "graphics/RenderDevice.h"
#include "ECS.h"

using namespace SomeVulkan;
using namespace ECS;

class GameMain {
private:
    std::shared_ptr< Renderer  > renderer{ nullptr };
public:
    void init( const std::shared_ptr< RenderDevice > &renderDevice ) {
    }

    void render( ) {
        renderer->render( );
    }

    void processMouseMove( double mouseX, double mouseY ) {
    }

    void processEvents( GLFWwindow *window ) {
        if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
            glfwSetWindowShouldClose( window, GL_TRUE );
        }
    }
};

int main( ) {
    auto *game = new GameMain { };


    Game g( 800, 600, "Some Vulkan", game );
    g.play( );


    delete game;

    return 0;
}