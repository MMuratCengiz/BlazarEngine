#pragma once

#include <BlazarCore/DynamicMemory.h>
#include <BlazarECS/ECS.h>
#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"
#include "BuiltinPrimitives.h"

#include <assimp/Importer.hpp>
#include <utility>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct ObjectBuffer
{
    uint64_t vertexCount;
    uint64_t indexCount;

    std::vector< std::pair< vk::Buffer, vma::Allocation > > animBufferList;

    std::pair< vk::Buffer, vma::Allocation > vertexBuffer;
    std::pair< vk::Buffer, vma::Allocation > indexBuffer;
    std::vector< uint32_t > indices;
};

struct ObjectBufferList
{
    std::vector< ObjectBuffer > buffers;
};


class MeshLoader
{
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

    ~MeshLoader( );
private:
    void loadModel( const ECS::CMesh &mesh );
    void onEachNode( ObjectBufferList &buffer, const aiScene *scene, const aiNode *pNode );
    void onEachMesh( ObjectBufferList &buffer, const aiMesh *mesh );
    void copyVertexBuffer( std::pair< vk::Buffer, vma::Allocation > &buffer, const Core::DynamicMemory &memory );
    void copyIndexBuffer( ObjectBuffer &bufferPart, const std::vector< uint32_t > &indices );
    void onEachAnimMesh( ObjectBuffer &buffer, const aiAnimMesh *pMesh );
    static Core::DynamicMemory getMeshMemory( const aiMesh *mesh, const aiAnimMesh *animMesh  ) ;
};

END_NAMESPACES