#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include "BuiltinPrimitives.h"
#include "IResourceProvider.h"

#define SUPPORTED_BONE_COUNT 4

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct AnimationData
{
    double ticksPerSeconds;
    double duration;
    std::vector< glm::mat4 > boneTransformations;
};

struct MeshGeometry
{
    // Shader Data
    bool hasIndices = false;
    bool hasColors = false;
    bool hasBoneData = false;

    uint32_t vertexCount;
    std::vector< float > vertices;
    std::vector< float > colors;
    std::vector< uint32_t > indices;

    // Internal Data
    std::vector< int > boneIndices;
    std::vector< float > boneWeights;

    std::vector< glm::mat4 > boneOffsetMatrices;
    std::unordered_map< std::string, AnimationData > animations;
};

struct SceneContext
{
    const aiScene * scene;
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
    PlainTrianglePrimitive plainTrianglePrimitive { };

public:
    AssetManager( );
    std::shared_ptr< ECS::IGameEntity > createEntity( const std::string &meshPath );
    const MeshGeometry &getMeshGeometry( const std::string &path );
    std::shared_ptr< SamplerDataAttachment > getImage( const std::string &path );
    ~AssetManager( );
private:
    void loadImage( const std::string& path );
    void loadModel( const std::shared_ptr< ECS::IGameEntity >& rootEntity, const std::string &path );

    void onEachNode( const SceneContext& context,
                     const std::shared_ptr< ECS::IGameEntity >& currentEntity,
                     const std::string& currentRootPath,
                     const aiScene *scene,
                     const aiNode *pNode );

    void onEachMesh(
            const SceneContext& context,
            const std::shared_ptr< ECS::CMesh >& meshComponent,
            const aiMesh *mesh );

    static void fillGeometryVertexData(  const SceneContext& context, MeshGeometry &geometry, const aiMesh *mesh, const aiAnimMesh *animMesh );
    static void fillGeometryBoneData(  const SceneContext& context, MeshGeometry &geometry, const aiMesh *pMesh, void *pVoid );
    static glm::mat4 aiMatToGLMMat( const aiMatrix4x4 & aiMat );
};

END_NAMESPACES