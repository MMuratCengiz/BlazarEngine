#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include "AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct JointTransformAction
{
    ChannelTransformType transformType;
    glm::vec3 transform;
    int targetJoint;
};

class AnimationStateSystem : public ECS::ISystem
{
private:
    AssetManager * assetManager;
public:
    explicit AnimationStateSystem( AssetManager * assetManager ) : assetManager( std::move( assetManager ) ) { }

    void frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;

    void entityTick( const std::shared_ptr< ECS::IGameEntity > &entity ) override;

    void frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;

    void cleanup( ) override;
private:
    void handleAnim( const std::shared_ptr< ECS::CAnimState > &anim );

    void handleAnimStateChange( const std::shared_ptr< ECS::CAnimState > &sharedPtr );

    void playAnim( const std::shared_ptr< ECS::CAnimState > &sharedPtr );

    void setStepInterpolation( const AnimationChannel &channel, const glm::vec3 &transform_0, MeshJoint &joint ) const;

    void setLinearInterpolation( const AnimationChannel &channel, const glm::vec3 &transform_0, const glm::vec3 &transform_1, const glm::quat &interpolationValue, MeshJoint &joint ) const;

    glm::mat4 getJointTransformMat( Core::TreeNode< MeshJoint >* node );
};

END_NAMESPACES