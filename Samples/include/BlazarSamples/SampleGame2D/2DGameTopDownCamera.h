#pragma once

#include <BlazarECS/ECS.h>
#include <BlazarScene/World.h>

using namespace BlazarEngine;

class TDGameTopDownCamera : public ECS::IGameEntity
{
private:
    Scene::World * world;

    float left;
    float right;
    float top;
    float bottom;
    float zoomX;
    float zoomY;
    bool panning = false;
    float prevX = 0.0f;
    float prevY = 0.0f;

public:
    TDGameTopDownCamera( Scene::World * world );
};