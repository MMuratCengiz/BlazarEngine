#pragma once

#include "../core/Common.h"
#include "../ecs/ISystem.h"
#include "../ecs/CMesh.h"
#include "ObjectBuffer.h"
#include "InstanceContext.h"
#include "../core/DynamicMemory.h"

#include <assimp/Importer.hpp>
#include <utility>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

NAMESPACES( SomeVulkan, Graphics )

class SMeshLoader : public ECS::ISystem< ECS::CMesh, ObjectBuffer > {
    Assimp::Importer importer;

    std::shared_ptr< InstanceContext > context;
    pCommandExecutor commandExecutor;

    std::unordered_map< std::string, ObjectBuffer > loadedModels;
public:
    inline explicit SMeshLoader( std::shared_ptr< InstanceContext > context, pCommandExecutor &commandExecutor )
    : context( std::move( context ) ), commandExecutor( commandExecutor ) { }

    void beforeFrame( ObjectBuffer &objectBuffer, const ECS::CMesh &mesh ) override;
private:
    void loadModel( const ECS::CMesh &mesh );
    void onEachNode( ObjectBuffer &buffer, const aiScene *scene, const aiNode *pNode );
    void onEachMesh( ObjectBuffer &buffer, const aiMesh *mesh );
    void copyVertexBuffer( ObjectBufferPart &bufferPart, const Core::DynamicMemory& memory );
    void copyIndexBuffer( ObjectBufferPart &bufferPart, const std::vector< uint64_t > &indices );
    void initStagingBuffer( std::pair< vk::Buffer, vma::Allocation > &stagingBuffer, const void * data, const uint64_t &size );
};

END_NAMESPACES