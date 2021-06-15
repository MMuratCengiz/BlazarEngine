#include "SampleAnimatedFox.h"

using namespace BlazarEngine;

namespace Sample
{

SampleAnimatedFox::SampleAnimatedFox( Scene::World *world )
{
    auto meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/fox.gltf" ) );

    addChild( meshEntities );

    BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( this, glm::vec3( 3.0f, 0.15f, 5.0f ) );
    BlazarEngine::Physics::PhysicsTransformSystem::setScaleRecursive( this, glm::vec3( 0.05f, 0.05f, 0.05f ) );

    setAnimState( meshEntities );
}

void SampleAnimatedFox::setAnimState( const std::shared_ptr< ECS::IGameEntity >& entity )
{
    auto animState = entity->getComponent< ECS::CAnimState >( );

    if ( animState == nullptr )
    {
        for ( const auto& child: entity->getChildren( ) )
        {
            setAnimState( child );
        }

        return;
    }
    entity->getComponent< ECS::CMaterial >( )->textures.push_back( { "/assets/textures/fox.png" });

    auto * surveyTransition = new ECS::CAnimFlowNode { };
    surveyTransition->animName = "Survey";

    animState->currentNode->transitions[ 1 ] = surveyTransition;
    animState->state = 1;
}

}