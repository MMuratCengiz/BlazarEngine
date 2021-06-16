//
// Created by Murat on 6/12/2021.
//

#define GLM_FORCE_RADIANS

#include <glm/gtx/quaternion.hpp>
#include "AnimationStateSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

void AnimationStateSystem::frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    for ( const std::shared_ptr< ECS::CAnimState > &animState: componentTable->getComponents< ECS::CAnimState >( ) )
    {
        handleAnim( animState );
    }
}

void AnimationStateSystem::handleAnim( const std::shared_ptr< ECS::CAnimState > &anim )
{
    if ( anim->state != anim->previousState )
    {
        handleAnimStateChange( anim );
    }

    playAnim( anim );
}

void AnimationStateSystem::playAnim( const std::shared_ptr< ECS::CAnimState > &anim )
{
    auto currentNode = anim->currentNode;

    MeshGeometry &geometry = assetManager->getMeshGeometry( anim->mesh->geometryRefIdx, "" );

    AnimationData &animation = geometry.animations[ currentNode->animName ];

    currentNode->currentPlayTime += ( float ) Core::Time::getDeltaTime( );

    for ( auto channel : animation.channels )
    {
        int frame = 0;
        for ( ; currentNode->currentPlayTime >= channel.keyFrames[ frame ] && frame < channel.keyFrames.size( ); frame++ );

        currentNode->lastPlayedNode = frame;

        if ( frame == channel.keyFrames.size( ) )
        {
            frame = 0;
            currentNode->currentPlayTime = 0;
        }

        float keyFrame_0 = channel.keyFrames[ frame ];
        float keyFrame_1;

        glm::vec4 transform_0 = channel.transform[ frame ];
        glm::vec4 transform_1;

        if ( frame + 1 == channel.keyFrames.size( ) )
        {
            keyFrame_1 = channel.keyFrames[ 0 ];
            transform_1 = channel.transform[ 0 ];
        }
        else
        {
            keyFrame_1 = channel.keyFrames[ frame + 1 ];
            transform_1 = channel.transform[ frame + 1 ];
        }

        auto meshJointNode = geometry.nodeTree.findNode( channel.targetJoint );

        if ( meshJointNode != nullptr )
        {
            auto interpolationValue = std::max( 0.0f, ( currentNode->currentPlayTime - keyFrame_0 ) / ( keyFrame_1 - keyFrame_0 ) );

            MeshNode &joint = meshJointNode->data;

            if ( channel.interpolationType == JointInterpolationType::Step )
            {
                setStepInterpolation( channel, transform_0, joint );
            }
            else if ( channel.interpolationType == JointInterpolationType::Linear )
            {
                setLinearInterpolation( channel, transform_0, transform_1, interpolationValue, joint );
            }
        }
    }

    anim->boneTransformations.resize( geometry.nodeTree.size( ), glm::mat4( 1.0f ) );

    auto meshGlobalTransform = getGlobalTransform( geometry, geometry.nodeTree.findNode( geometry.meshNodeIdx ) );
    auto inverseMeshTransform = glm::inverse( meshGlobalTransform );

    for ( int i = 0; i < geometry.joints.size( ); ++i )
    {
        anim->boneTransformations[ i ] =
                getBoneTransform( anim, geometry, geometry.nodeTree.findNode( geometry.joints[ i ] ), inverseMeshTransform );
    }
}

glm::mat4 AnimationStateSystem::getBoneTransform( const std::shared_ptr< ECS::CAnimState > &anim, MeshGeometry &geometry, Core::TreeNode< MeshNode, int > *node, const glm::mat4 &inverseMeshTransform )
{
    auto inverseWorldMatrix = glm::inverse( getGlobalTransform( geometry, node->parent ) );
    auto globalTransform = getGlobalTransform( geometry, node );

    auto jointMatrix = globalTransform * node->data.inverseBindMatrix;
    return /*inverseWorldMatrix * */jointMatrix;
}

glm::mat4 AnimationStateSystem::getGlobalTransform( MeshGeometry &geometry, Core::TreeNode< MeshNode, int > *node )
{
    if ( node == nullptr /*|| node == geometry.nodeTree.getRoot( ) */)
    {
        return glm::mat4( 1.0f );
    }

    return getGlobalTransform( geometry, node->parent ) * node->data.getTransform( );
}

void AnimationStateSystem::setLinearInterpolation( const AnimationChannel &channel, const glm::vec4 &transform_0, const glm::vec4 &transform_1, const float &interpolationValue, MeshNode &joint ) const
{
    if ( channel.transformType == Rotation )
    {
        joint.rotation = glm::normalize( glm::slerp( Core::Utilities::vecToQuat( transform_0 ), Core::Utilities::vecToQuat( transform_1 ), interpolationValue ) );
    }
    else if ( channel.transformType == Scale )
    {
        joint.scale = glm::vec3( glm::mix( transform_0, transform_1, interpolationValue ) );
    }
    else if ( channel.transformType == Translation )
    {
        joint.translation = glm::vec3( glm::mix( transform_0, transform_1, interpolationValue ) );
    }
}

void AnimationStateSystem::setStepInterpolation( const AnimationChannel &channel, const glm::vec4 &transform_0, MeshNode &joint ) const
{
    if ( channel.transformType == Rotation )
    {
        auto rotation = glm::quat( transform_0 );
        joint.rotation = glm::mat4( rotation );
    }
    else if ( channel.transformType == Scale )
    {
        joint.scale = glm::vec3( transform_0 );
    }
    else if ( channel.transformType == Translation )
    {
        joint.translation = glm::vec3( transform_0 );
    }
}

void AnimationStateSystem::handleAnimStateChange( const std::shared_ptr< ECS::CAnimState > &anim )
{
    anim->currentNode->currentPlayTime = 0.0;

    anim->previousState = anim->state;
    anim->currentNode = anim->currentNode->transitions[ anim->state ];
}

void AnimationStateSystem::entityTick( const std::shared_ptr< ECS::IGameEntity > &entity )
{ }

void AnimationStateSystem::frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{

}

void AnimationStateSystem::cleanup( )
{

}

END_NAMESPACES