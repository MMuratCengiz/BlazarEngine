//
// Created by Murat on 6/12/2021.
//

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
        } else
        {
            keyFrame_1 = channel.keyFrames[ frame + 1 ];
            transform_1 = glm::vec3( channel.transform[ frame + 3 ], channel.transform[ frame + 4 ], channel.transform[ frame + 5 ] );
        }

        auto meshJointNode = geometry.jointTree.findNode( channel.targetJoint );

        if ( meshJointNode != nullptr )
        {
            auto interpolationValue = glm::vec3( ( currentNode->currentPlayTime - keyFrame_0 ) / ( keyFrame_1 - keyFrame_0 ) );

            MeshJoint &joint = meshJointNode->data;

            if ( channel.interpolationType == JointInterpolationType::Step )
            {
                setStepInterpolation( channel, transform_0, joint );
            } else if ( channel.interpolationType == JointInterpolationType::Linear )
            {
                setLinearInterpolation( channel, transform_0, transform_1, interpolationValue, joint );
            }
        }
    }

    for ( auto node: geometry.jointTree.flattenTree( true ) )
    {
        if ( anim->boneTransformations.size() <= node->id )
        {
            anim->boneTransformations.resize( node->id + 1 );
        }

        anim->boneTransformations[ node->id ] = getJointTransformMat( node ) * node->data.inverseBindMatrix;
    }
}

glm::mat4 AnimationStateSystem::getJointTransformMat( Core::TreeNode< MeshJoint > * node )
{
    if ( node == nullptr )
    {
        return glm::mat4( 1.0f );
    }

    JointTransform &jointTransform = node->data.keyFrameTransform;

    glm::mat4 translationMatrix = glm::translate( glm::mat4( 1.0f ), jointTransform.translation );
    glm::mat4 rotationMatrix = glm::toMat4( jointTransform.rotation );
    glm::mat4 scaleMatrix = glm::scale( glm::mat4( 1.0f ), jointTransform.scale );

    return getJointTransformMat( node->parent ) * translationMatrix * rotationMatrix * scaleMatrix;
}

void AnimationStateSystem::setLinearInterpolation( const AnimationChannel &channel, const glm::vec3 &transform_0, const glm::vec3 &transform_1, const glm::quat &interpolationValue, MeshJoint &joint ) const
{
    if ( channel.transformType == Rotation )
    {
        joint.keyFrameTransform.rotation = glm::slerp( glm::quat( transform_0 ), glm::quat( transform_1 ), interpolationValue.x );
    } else if ( channel.transformType == Scale )
    {
        joint.keyFrameTransform.scale = transform_0 + interpolationValue * ( transform_1 - transform_0 );
    } else if ( channel.transformType == Translation )
    {
        joint.keyFrameTransform.translation = transform_0 + interpolationValue * ( transform_1 - transform_0 );
    }
}

void AnimationStateSystem::setStepInterpolation( const AnimationChannel &channel, const glm::vec3 &transform_0, MeshJoint &joint ) const
{
    if ( channel.transformType == Rotation )
    {
        joint.keyFrameTransform.rotation = glm::quat( transform_0 );
    } else if ( channel.transformType == Scale )
    {
        joint.keyFrameTransform.scale = transform_0;
    } else if ( channel.transformType == Translation )
    {
        joint.keyFrameTransform.translation = transform_0;
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