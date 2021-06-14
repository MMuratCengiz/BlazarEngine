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

    float currentPlayTime = 0.0;
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