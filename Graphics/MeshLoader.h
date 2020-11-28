#pragma once

#include "../Core/Common.h"
#include "../Core/DynamicMemory.h"
#include "../ECS/ISystem.h"
#include "../ECS/CMesh.h"
#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"
#include "BuiltinPrimitives.h"

#include <assimp/Importer.hpp>
#include <utility>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

NAMESPACES( SomeVulkan, Graphics )

struct ObjectBuffer {
    uint64_t vertexCount;
    uint64_t indexCount;

    std::pair< vk::Buffer, vma::Allocation > vertexBuffer;
    std::pair< vk::Buffer, vma::Allocation > indexBuffer;
    std::vector< uint32_t > indices;
};

struct ObjectBufferList {
    std::vector< ObjectBuffer > buffers;
};


class MeshLoader {
    Assimp::Importer importer;

    LightedCubePrimitive lightedCubePrimitive;
    PlainCubePrimitive plainCubePrimitive;
    std::shared_ptr< InstanceContext > context;
    pCommandExecutor commandExecutor;

    std::unordered_map< std::string, ObjectBufferList > loadedModels;

public:
    explicit MeshLoader( std::shared_ptr< InstanceContext > context, pCommandExecutor &commandExecutor );

    void cache( const ECS::CMesh &mesh );
    void load( ObjectBufferList &objectBuffer, const ECS::CMesh &mesh );

    ~MeshLoader();
private:
    void loadModel( const ECS::CMesh &mesh );
    void onEachNode( ObjectBufferList &buffer, const aiScene *scene, const aiNode *pNode );
    void onEachMesh( ObjectBufferList &buffer, const aiMesh *mesh );
    void copyVertexBuffer( ObjectBuffer &bufferPart, const Core::DynamicMemory& memory );
    void copyIndexBuffer( ObjectBuffer &bufferPart, const std::vector< uint32_t > &indices );
};

END_NAMESPACES