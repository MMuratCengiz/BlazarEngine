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

    currentNode->currentPlayTime += ( float ) Core::Time::getDeltaTime( );

    MeshGeometry &geometry = assetManager->getMeshGeometry( anim->mesh->geometryRefIdx, "" );

    AnimationData &animation = geometry.animations[ currentNode->animName ];

    for ( auto channel : animation.channels )
    {
        int frame = 0;
        for ( ; currentNode->currentPlayTime >= channel.keyFrames[ frame ] && frame < channel.keyFrames.size( ); frame++ );

        if ( frame == channel.keyFrames.size( ) )
        {
            frame = 0;
            currentNode->currentPlayTime = 0;
        }

        float keyFrame_0 = channel.keyFrames[ frame ];
        float keyFrame_1;

        glm::vec3 transform_0 = glm::vec3( channel.transform[ frame ], channel.transform[ frame + 1 ], channel.transform[ frame + 2 ] );
        glm::vec3 transform_1;

        if ( frame + 1 == channel.keyFrames.size( ) )
        {
            keyFrame_1 = channel.keyFrames[ 0 ];
            transform_1 = glm::vec3( channel.transform[ 0 ], channel.transform[ 1 ], channel.transform[ 2 ] );
        }
        else
        {
            keyFrame_1 = channel.keyFrames[ frame + 1 ];
            transform_1 = glm::vec3( channel.transform[ frame + 3 ], channel.transform[ frame + 4 ], channel.transform[ frame + 5 ] );
        }

        auto meshJointNode = geometry.nodeTree.findNode( channel.targetJoint );

        if ( meshJointNode != nullptr )
        {
            auto interpolationValue = glm::vec3( ( currentNode->currentPlayTime - keyFrame_0 ) / ( keyFrame_1 - keyFrame_0 ) );

            MeshNode &joint = meshJointNode->data;

            if ( channel.interpolationType == JointInterpolationType::Step )
            {
                setStepInterpolation( channel, transform_0, joint );
            }
            else if ( channel.interpolationType == JointInterpolationType::Linear )
            {
                setLinearInterpolation( channel, transform_0, transform_1, interpolationValue, joint );
            }

            meshJointNode->data.transform = Core::Utilities::getTRSMatrix( joint.translation, joint.rotation, joint.scale );
        }
    }

    anim->boneTransformations.resize( geometry.nodeTree.size( ) );

    for ( int i = 0; i < geometry.joints.size( ); ++i )
    {
        setBoneTransforms( anim, geometry, geometry.nodeTree.findNode( geometry.joints[ i ] ) );
    }
}

void AnimationStateSystem::setBoneTransforms( const std::shared_ptr< ECS::CAnimState > &anim, MeshGeometry &geometry, Core::TreeNode< MeshNode, int > *node )
{
    auto globalTransform = getGlobalTransform( geometry, node->id );
    auto meshGlobalTransform = getGlobalTransform( geometry, geometry.meshNodeIdx );
    auto inverseGlobalTransform = glm::inverse( meshGlobalTransform );

    anim->boneTransformations[ node->data.boneTransformAccessIdx ] = inverseGlobalTransform * globalTransform * node->data.inverseBindMatrix;

    for ( auto child: node->children )
    {
        setBoneTransforms( anim, geometry, child );
    }
}

glm::mat4 AnimationStateSystem::getGlobalTransform( MeshGeometry& geometry, const int& nodeIdx )
{
    Core::TreeNode< MeshNode, int > *pNode = geometry.nodeTree.findNode( nodeIdx );

    if ( pNode == geometry.nodeTree.getRoot( ) || pNode == nullptr )
    {
        return glm::mat4( 1.0f );
    }

    return getGlobalTransform( geometry, pNode->parent->id ) * pNode->data.transform;
}

glm::mat4 AnimationStateSystem::getJointTransform( MeshGeometry& geometry, const int& jointIdx )
{
    Core::TreeNode< MeshNode, int > *pNode = geometry.nodeTree.findNode( jointIdx );

    if ( pNode == geometry.nodeTree.getRoot( ) || pNode == nullptr )
    {
        return glm::mat4( 1.0f );
    }

    return getGlobalTransform( geometry, pNode->parent->id ) * pNode->data.animTransform * pNode->data.transform;
}

void AnimationStateSystem::setLinearInterpolation( const AnimationChannel &channel, const glm::vec3 &transform_0, const glm::vec3 &transform_1, const glm::quat &interpolationValue, MeshNode &joint ) const
{
    if ( channel.transformType == Rotation )
    {
        joint.rotation = glm::normalize( glm::slerp( glm::quat( transform_0 ), glm::quat( transform_1 ), interpolationValue.x ) );
    }
    else if ( channel.transformType == Scale )
    {
        joint.scale = glm::mix( glm::vec4( transform_0, 0.0f ), glm::vec4( transform_1, 0.0f ), interpolationValue.x );
    }
    else if ( channel.transformType == Translation )
    {
        joint.translation = glm::mix( glm::vec4( transform_0, 0.0f ), glm::vec4( transform_1, 0.0f ), interpolationValue.x );
    }
}

void AnimationStateSystem::setStepInterpolation( const AnimationChannel &channel, const glm::vec3 &transform_0, MeshNode &joint ) const
{
    if ( channel.transformType == Rotation )
    {
        joint.rotation = glm::quat( transform_0 );
    }
    else if ( channel.transformType == Scale )
    {
        joint.scale = transform_0;
    }
    else if ( channel.transformType == Translation )
    {
        joint.translation = transform_0;
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