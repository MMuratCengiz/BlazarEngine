#pragma once

#include <BlazarCore/Common.h>
#include <BlazarCore/Utilities.h>
#include <BlazarECS/ECS.h>
#include "AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

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

    void setStepInterpolation( const AnimationChannel &channel, const glm::vec4 &transform_0, MeshNode &joint ) const;

    void setLinearInterpolation( const AnimationChannel &channel, const glm::vec4 &transform_0, const glm::vec4 &transform_1, const float &interpolationValue, MeshNode &joint ) const;

    glm::mat4 getBoneTransform( const std::shared_ptr< ECS::CAnimState > &anim, MeshGeometry& geometry, Core::TreeNode< MeshNode, int > *node, const glm::mat4& inverseMeshTransform );

    glm::mat4 getGlobalTransform( MeshGeometry& geometry, Core::TreeNode< MeshNode, int > *node );
};

END_NAMESPACES