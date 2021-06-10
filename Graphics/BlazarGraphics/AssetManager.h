#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

#include "BuiltinPrimitives.h"
#include "IResourceProvider.h"
#include <tiny_gltf.h>

#define SUPPORTED_BONE_COUNT 4

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum PrimitiveDrawMode
{
    Point,
    Line,
    Triangle
};

struct AnimationData
{
    double ticksPerSeconds;
    double duration;
    std::vector< glm::mat4 > boneTransformations;
};

struct SubMeshGeometry
{
    PrimitiveDrawMode drawMode;

    uint32_t vertexCount;
    std::vector< unsigned int > indices;

    std::vector< float > vertices;
    std::vector< float > normals;
    std::vector< float > colors;
    std::vector< float > tangents;
    std::vector< float > textureCoordinates;

    std::vector< float > dataRaw;
};

struct MeshGeometry
{
    std::vector< SubMeshGeometry > subGeometries;

    // Internal Data
    std::vector< int > boneIndices;
    std::vector< float > boneWeights;

    std::vector< glm::mat4 > boneOffsetMatrices;
    std::unordered_map< std::string, AnimationData > animations;
};

struct SceneContext
{
    tinygltf::Model model;
};

class AssetManager
{
private:
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
    void loadImage( const std::string &path );

    void loadModel( const std::shared_ptr< ECS::IGameEntity > &rootEntity, const std::string &path );

    void onEachScene( const SceneContext &context, const std::shared_ptr< ECS::IGameEntity > &currentEntity, const std::string &currentRootPath, const tinygltf::Model &model, const int &currentNode );

    void onEachMesh( const SceneContext &context, const std::shared_ptr< ECS::CMesh > &meshComponent, const tinygltf::Model &model, const int &mesh );

    template< typename SourceType, typename BufferType = SourceType >
    void copyAccessorToVector( std::vector< SourceType > &targetData, const tinygltf::Model &model, int accessorIdx )
    {
        tinygltf::Accessor positionAccessor = model.accessors[ accessorIdx ];
        tinygltf::BufferView bufferView = model.bufferViews[ positionAccessor.bufferView ];
        tinygltf::Buffer buffer = model.buffers[ bufferView.buffer ];

        size_t numOfComponents = tinygltf::GetNumComponentsInType( positionAccessor.type );
        size_t size = positionAccessor.count * numOfComponents;
        std::vector< BufferType > result( size );

        unsigned int componentSize = tinygltf::GetComponentSizeInBytes( positionAccessor.componentType );
        unsigned int currentOffset = positionAccessor.byteOffset + bufferView.byteOffset;

        unsigned char *data = buffer.data.data( );

        unsigned int stride = std::max( bufferView.byteStride, numOfComponents * componentSize );

        bool dataClumped = numOfComponents == 1 && bufferView.byteStride == 0;
        if ( dataClumped )
        {
            // Data clumped
            memcpy( &result[ 0 ], data + currentOffset, componentSize * size );
        }

        for ( uint32_t iter = currentOffset, idx = 0; idx < size && !dataClumped; iter += stride, idx += numOfComponents )
        {
            memcpy( &result[ idx ], data + iter, componentSize * numOfComponents );
        }

        if ( typeid( SourceType ) != typeid( BufferType ) )
        {
            std::transform( result.begin( ), result.end( ), std::back_inserter( targetData ), [ ]( BufferType b )
            { return ( SourceType ) b; } );
        } else
        {
            targetData.insert( targetData.end( ), result.begin( ), result.end( ) );
        }
    }

    template< typename SourceType >
    void copyAccessorToVectorTransformed( std::vector< SourceType > &targetData, const tinygltf::Model &model, int accessorIdx )
    {
        tinygltf::Accessor positionAccessor = model.accessors[ accessorIdx ];

        switch ( positionAccessor.componentType )
        {
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                copyAccessorToVector< SourceType, short >( targetData, model, accessorIdx );
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                copyAccessorToVector< SourceType, unsigned short >( targetData, model, accessorIdx );
                break;
            case TINYGLTF_COMPONENT_TYPE_INT:
                copyAccessorToVector< SourceType, int >( targetData, model, accessorIdx );
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                copyAccessorToVector< SourceType, unsigned int >( targetData, model, accessorIdx );
                break;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                copyAccessorToVector< SourceType, float >( targetData, model, accessorIdx );
                break;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                copyAccessorToVector< SourceType, double >( targetData, model, accessorIdx );
                break;
        }
    }

/*    static void fillGeometryVertexData( const SceneContext &context, MeshGeometry &geometry, const aiMesh *mesh, const aiAnimMesh *animMesh );

    static void fillGeometryBoneData( const SceneContext &context, MeshGeometry &geometry, const aiMesh *pMesh, void *pVoid );*/
    static void packSubGeometry( SubMeshGeometry &geometry );
};

END_NAMESPACES