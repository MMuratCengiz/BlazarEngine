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
#include "CMesh.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

#define ANIM_STATE_ANY -1

enum NodeType
{
    Begin,
    FlowNode,
    End
};

struct CAnimFlowNode;

struct CAnimFlowNode
{
    NodeType nodeType = NodeType::FlowNode;

    std::string animName;

    std::unordered_map< int, CAnimFlowNode * > transitions;

    double currentPlayTime = 0.0;

    int lastPlayedFrame = -1;

    ~CAnimFlowNode()
    {
        for ( auto transition: transitions )
        {
            delete transition.second;
        }
    }
};

struct CAnimState : IComponent
{
    std::shared_ptr< CMesh > mesh; // Performance ref

    std::vector< glm::mat4 > boneTransformations;

    CAnimFlowNode * beginNode = new CAnimFlowNode { NodeType::Begin, "", { } };

    CAnimFlowNode * currentNode = beginNode;

    int state = 0;
    int previousState = 0;

    ~CAnimState() override
    {
        delete beginNode;
    }

    BLAZAR_COMPONENT_CUSTOM_DESTRUCTOR(CAnimState)
};

END_NAMESPACES