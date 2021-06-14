#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

#include "BuiltinPrimitives.h"
#include "IResourceProvider.h"
#include <tiny_gltf.h>
#include <BlazarCore/SimpleTree.h>

#define SUPPORTED_BONE_COUNT 4

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum PrimitiveDrawMode
{
    Point,
    Line,
    Triangle
};

enum JointInterpolationType
{
    Linear,
    Step,
    CubicSpline
};

enum ChannelTransformType
{
    Translation,
    Rotation,
    Scale,
    Weights // Not yet implemented
};

struct AnimationChannel
{
    int targetJoint;

    std::vector< float > keyFrames;
    std::vector< float > transform;
    ChannelTransformType transformType;
    JointInterpolationType interpolationType;
};

struct AnimationData
{
    std::vector< AnimationChannel > channels;
};

struct JointTransform
{
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
};

struct MeshJoint
{
    glm::mat4 inverseBindMatrix;

    JointTransform baseTransform = { };
    JointTransform keyFrameTransform = { };
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
    std::vector< float > boneIndices;
    std::vector< float > boneWeights;

    std::vector< float > dataRaw;
};

struct MeshGeometry
{
    std::vector< SubMeshGeometry > subGeometries;

    // Internal Data

    Core::SimpleTree< MeshJoint >  jointTree = { };

    std::unordered_map< std::string, AnimationData > animations;
};

struct SceneContext
{
    tinygltf::Model model;

    std::unordered_map< std::string, AnimationData > animations;
    std::shared_ptr< ECS::IGameEntity > rootEntity;
};

struct MeshContext
{
    int geometryIdx;
    std::shared_ptr< ECS::IGameEntity > entity;
};

class AssetManager
{
private:
    // todo instead of having a map, use access by index by storing the index data in the component
    std::vector< MeshGeometry > geometryTable;
    std::unordered_map< std::string, std::shared_ptr< SamplerDataAttachment > > imageMap;

    LitCubePrimitive litCubePrimitive { };
    PlainCubePrimitive plainCubePrimitive { };
    PlainSquarePrimitive plainSquarePrimitive { };
    PlainTrianglePrimitive plainTrianglePrimitive { };

public:
    AssetManager( );

    std::shared_ptr< ECS::IGameEntity > createEntity( const std::string &meshPath );

    MeshGeometry &getMeshGeometry( const int &geometryIdx, const std::string& builtinPrimitive );

    std::shared_ptr< SamplerDataAttachment > getImage( const std::string &path );

    int findBuiltinPrimitiveIdx( const std::string& primitiveName ) const;

    ~AssetManager( );

private:
    void loadImage( const std::string &path );

    void loadModel( const std::shared_ptr< ECS::IGameEntity > &rootEntity, const std::string &path );

    void generateAnimationData( SceneContext &sceneContext );

    void onEachNode( const SceneContext &context, const std::shared_ptr< ECS::IGameEntity > &currentEntity, const std::string &currentRootPath, const int &currentNode );

    void onEachMesh( const SceneContext &sceneContext, MeshContext meshContext, const int &meshIdx );

    void onEachSkin( const SceneContext &sceneContext, MeshContext meshContext, const int &skinIdx );

    void onEachJoint( const SceneContext &sceneContext, MeshContext meshContext, const std::vector< glm::mat4 >& inverseBindMatrices, const int& parentIdx, const int &jointIdx );

    static int tryGetPrimitiveAttribute( const tinygltf::Primitive& primitive, const std::string & attribute );

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
        if ( accessorIdx < 0 )
        {
            return;
        }

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

    static glm::mat4 flatMatToGLMMat( const std::vector< float > &matFlat, int offset );

    static void packSubGeometry( SubMeshGeometry &geometry );

    void onEachChannel( const tinygltf::Model &model, const tinygltf::Animation &animation, AnimationData &animationData, const tinygltf::AnimationChannel &channel );
};

END_NAMESPACES