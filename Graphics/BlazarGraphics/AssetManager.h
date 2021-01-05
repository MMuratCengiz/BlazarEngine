#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include "BuiltinPrimitives.h"
#include "IResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct MeshGeometry
{
    bool hasIndices = false;
    uint32_t vertexCount;
    std::vector< float > vertices;
    std::vector< uint32_t > indices;
};

class AssetManager
{
private:
    Assimp::Importer importer;
    std::unordered_map< std::string, MeshGeometry > geometryMap;
    std::unordered_map< std::string, std::shared_ptr< SamplerDataAttachment > > imageMap;

    LitCubePrimitive litCubePrimitive { };
    PlainCubePrimitive plainCubePrimitive { };
    PlainSquarePrimitive plainSquarePrimitive { };
public:
    AssetManager( );
    std::shared_ptr< ECS::IGameEntity > createEntity( const std::string &meshPath );
    const MeshGeometry &getMeshGeometry( const std::string &path );
    std::shared_ptr< SamplerDataAttachment > getImage( const std::string &path );
    ~AssetManager( );
private:
    void loadImage( const std::string& path );
    void loadModel( const std::shared_ptr< ECS::IGameEntity >& rootEntity, const std::string &path );
    void onEachNode( const std::shared_ptr< ECS::IGameEntity >& currentEntity, const std::string& currentRootPath, const aiScene *scene, const aiNode *pNode );
    void onEachMesh( const std::shared_ptr< ECS::CMesh >& meshComponent, const aiMesh *mesh );
    static void fillGeometryVertexData( MeshGeometry &geometry, const aiMesh *mesh, const aiAnimMesh *animMesh );
};

END_NAMESPACES