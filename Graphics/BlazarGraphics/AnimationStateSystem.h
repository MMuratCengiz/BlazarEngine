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

    glm::mat4 getBoneTransform( MeshGeometry& geometry, Core::TreeNode< MeshNode, int > *meshNode, Core::TreeNode< MeshNode, int > *node );
};

END_NAMESPACES