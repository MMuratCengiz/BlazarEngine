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

#include "BuiltinPrimitives.h"
#include "IResourceProvider.h"
#include <tiny_gltf.h>
#include <BlazarCore/SimpleTree.h>
#include "boost/algorithm/string/case_conv.hpp"
#include <BlazarCore/Logger.h>

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
    std::vector< glm::vec4 > transform;

    ChannelTransformType transformType;
    JointInterpolationType interpolationType;
};

struct AnimationData
{
    std::vector< AnimationChannel > channels;
};

struct MeshNode
{
    glm::vec3 translation = glm::vec3( 0.0f );
    glm::quat rotation = glm::quat( glm::mat4( 1.0f ) );
    glm::vec3 scale = glm::vec3( 1.0f );
    glm::mat4 inverseBindMatrix = glm::mat4( 1.0f );
    glm::mat4 globalTransform = glm::mat4( 1.0f );
    glm::mat4 inverseGlobalTransform = glm::mat4( 1.0f );

    [[nodiscard]] glm::mat4 getTransform( ) const
    {
        auto T = glm::translate( glm::mat4( 1.0f ), translation );
        auto R = T * glm::mat4( rotation );
        auto S = glm::scale( R, scale );
        return S;
    }
};

struct SubMeshGeometry
{
    PrimitiveDrawMode drawMode;

    uint32_t vertexCount;
    std::vector< unsigned int > indices;
    std::vector< unsigned int > joints;

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
    int meshNodeIdx;

    std::vector< SubMeshGeometry > subGeometries;

    // Internal Data

    std::vector< int > joints = { };
    std::shared_ptr< Core::SimpleTree< MeshNode > > nodeTree = nullptr;

    std::unordered_map< std::string, AnimationData > animations;

    tinygltf::Model model;

    void updateWorldTransforms( Core::TreeNode< MeshNode > *parent, Core::TreeNode< MeshNode > *node )
    {
        node->data.globalTransform = node->data.getTransform( );

        if ( parent != nullptr )
        {
            node->data.globalTransform = parent->data.globalTransform * node->data.getTransform( );
        }

        node->data.inverseGlobalTransform = glm::inverse( node->data.globalTransform );

        for ( auto child: node->children )
        {
            updateWorldTransforms( node, child );
        }
    }

    void updateWorldTransforms( )
    {
        updateWorldTransforms( nullptr, nodeTree->getRoot( ) );
    }
};

struct MeshContext
{
    int geometryIdx;
    int meshNodeIdx;
};

struct SceneContext
{
    tinygltf::Model model;

    std::unordered_map< std::string, AnimationData > animations;
    ECS::IGameEntity * rootEntity;

    std::shared_ptr< Core::SimpleTree< MeshNode > > nodeTree;

    std::unordered_map< int, MeshContext > meshContextMap;
    bool multiMeshNodes;

    std::string gltfModelDirectory;
};

class AssetManager
{
private:
    const int LIT_CUBE_GEOMETRY_IDX = 0;
    const int PLAIN_CUBE_GEOMETRY_IDX = 1;
    const int PLAIN_SQUARE_GEOMETRY_IDX = 2;
    const int PLAIN_TRIANGLE_GEOMETRY_IDX = 3;

    std::vector< MeshGeometry > geometryTable;
    std::unordered_map< std::string, std::unique_ptr< SamplerDataAttachment > > imageMap;

    LitCubePrimitive litCubePrimitive { };
    PlainCubePrimitive plainCubePrimitive { };
    PlainSquarePrimitive plainSquarePrimitive { };
    PlainTrianglePrimitive plainTrianglePrimitive { };

public:
    AssetManager( );

    std::unique_ptr< ECS::IGameEntity > createEntity( const std::string &meshPath );
    void createEntity( ECS::IGameEntity * attachToEntity, const std::string &meshPath );

    MeshGeometry &getMeshGeometry( const int &geometryIdx );
    MeshGeometry &getPrimitive( const PrimitiveType& primitive );

    std::unique_ptr< SamplerDataAttachment > getImage( const std::string &path );

    ~AssetManager( );

private:
    void loadImage( const std::string &path );

    void loadModel( ECS::IGameEntity * rootEntity, const std::string &path );

    void generateAnimationData( SceneContext &sceneContext );

    void onEachNode( SceneContext &context, ECS::IGameEntity * entity, const std::string &currentRootPath, const int &parentNode, const int &currentNode );

    void onEachMesh( SceneContext &context, const std::string &currentRootPath, Core::TreeNode< MeshNode > *currentNode );

    void generateMeshData( SceneContext &context, const int &currentNode );

    void onEachSkin( SceneContext &sceneContext, const int &meshIdx, const int &skinIdx );

    void addNode( SceneContext &sceneContext, int parent, int nodeId );

    static int tryGetPrimitiveAttribute( const tinygltf::Primitive &primitive, const std::string &attribute );

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

        bool dataClumped = bufferView.byteStride == 0;
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
            { return static_cast< SourceType >( b ); } );
        }
        else
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

    void generateNormals( SubMeshGeometry &subMeshGeometry ) const;

    void attachMaterialData( SceneContext &context, ECS::IGameEntity * sharedPtr, int mesh );
};

END_NAMESPACES