#pragma once

#include <BlazarScene/World.h>
#include "2DGameTopDownCamera.h"

using namespace BlazarEngine;

class TDGameScene : public Scene::IPlayable
{
private:
    Scene::World *world;
    std::unique_ptr< Scene::Scene > scene;
    std::unique_ptr< ECS::IGameEntity > box;
    std::unique_ptr< TDGameTopDownCamera > camera;
    std::unique_ptr< Graphics::Pass > presentPass;
public:
    inline explicit TDGameScene( Scene::World *world ) : world( world )
            { }

    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};