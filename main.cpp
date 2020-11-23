#include <iostream>
#include "Core/Game.h"

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
 //   std::vector< Graphics::ShaderInfo > shaders{ };
	//shaders.emplace_back( Graphics::ShaderInfo{ vk::ShaderStageFlagBits::eVertex, PATH( "/shaders/spirv/vertex/default.spv" ) } );
	//shaders.emplace_back( Graphics::ShaderInfo{ vk::ShaderStageFlagBits::eFragment, PATH( "/shaders/spirv/fragment/default.spv" ) } );

 //   Graphics::GLSLShaderSet shader( shaders );

    auto *game = new GameMain { };

    Game g( 800, 600, "Some Vulkan", game );
    g.play( );

    delete game;

    return 0;
}
