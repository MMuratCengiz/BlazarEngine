/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define GLM_FORCE_RADIANS

#include <BlazarGraphics/AnimationStateSystem.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

void AnimationStateSystem::frameStart( ECS::ComponentTable * componentTable )
{
    for ( const auto& animState: componentTable->getComponents< ECS::CAnimState >( ) )
    {
        handleAnim( animState );
    }
}

void AnimationStateSystem::handleAnim( ECS::CAnimState * anim )
{
    if ( anim->state != anim->previousState )
    {
        handleAnimStateChange( anim );
    }

    playAnim( anim );
}

void AnimationStateSystem::playAnim( ECS::CAnimState * anim )
{
    auto currentNode = anim->currentNode;

    MeshGeometry &geometry = assetManager->getMeshGeometry( anim->mesh->geometryRefIdx );

    AnimationData &animation = geometry.animations[ currentNode->animName ];

    currentNode->currentPlayTime += Core::Time::getDeltaTime( );

    int updateCheck = 0;

    for ( auto channel : animation.channels )
    {
        int frame = 0;
        for ( float keyFrame: channel.keyFrames )
        {
            if ( currentNode->currentPlayTime == 0 || currentNode->currentPlayTime < keyFrame )
            {
                break;
            }

            frame++;
        }

        if ( updateCheck == 0 && currentNode->lastPlayedFrame != -1 && currentNode->lastPlayedFrame == frame  )
        {
            return;
        }

        updateCheck++;
        currentNode->lastPlayedFrame = frame;

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

        auto meshJointNode = geometry.nodeTree->findNode( channel.targetJoint );

        if ( meshJointNode != nullptr )
        {
            auto interpolationValue = std::max( 0.0f, ( ( float ) currentNode->currentPlayTime - keyFrame_0 ) / ( keyFrame_1 - keyFrame_0 ) );

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

    geometry.updateWorldTransforms( );

    auto meshNode = geometry.nodeTree->findNode( geometry.meshNodeIdx );

    anim->boneTransformations.clear( );
    anim->boneTransformations.resize( geometry.joints.size( ), glm::mat4( 1.0f ) );

    for ( int i = 0; i < geometry.joints.size( ); ++i )
    {
        anim->boneTransformations[ i ] = getBoneTransform( geometry, meshNode, geometry.nodeTree->findNode( geometry.joints[ i ] ) );
    }
}

glm::mat4 AnimationStateSystem::getBoneTransform( MeshGeometry &geometry, Core::TreeNode< MeshNode, int > *meshNode, Core::TreeNode< MeshNode, int > *node )
{
    float gMatrix[16];
    float jMatrix[16];
    float ibm[16];
    float igt[16];

    memcpy( &gMatrix[ 0 ], glm::value_ptr( node->data.globalTransform ), 16 * 4 );
    memcpy( &ibm[ 0 ], glm::value_ptr( node->data.inverseBindMatrix ), 16 * 4 );
    memcpy( &igt[ 0 ], glm::value_ptr( node->parent->data.inverseGlobalTransform ), 16 * 4 );

    auto localMatrix = node->data.getTransform( );
    auto jointMatrix = node->data.globalTransform * node->data.inverseBindMatrix;

    memcpy( &jMatrix[ 0 ], glm::value_ptr( jointMatrix ), 16 * 4 );

    if ( node->parent != nullptr )
    {
        jointMatrix = meshNode->data.inverseGlobalTransform * jointMatrix;
    }

    return jointMatrix;
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

void AnimationStateSystem::handleAnimStateChange( ECS::CAnimState * anim  )
{
    anim->currentNode->currentPlayTime = 0.0;

    anim->previousState = anim->state;

    anim->currentNode = anim->nodes[ anim->currentNode->transitions[ anim->state ] ];
}

void AnimationStateSystem::entityTick( ECS::IGameEntity * entity )
{ }

void AnimationStateSystem::frameEnd( ECS::ComponentTable * componentTable )
{

}

void AnimationStateSystem::cleanup( )
{

}

END_NAMESPACES