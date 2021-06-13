#include "AssetManager.h"
#include "GraphicsException.h"
#include <glm/gtx/quaternion.hpp>
#include <boost/format.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

AssetManager::AssetManager( )
{
    const std::string &litCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube );
    geometryMap[ litCubePath ] = { };
    MeshGeometry &litCube = geometryMap[ litCubePath ];
    SubMeshGeometry &litCubeSubMesh = litCube.subGeometries.emplace_back( );
    litCubeSubMesh.dataRaw = litCubePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    litCubeSubMesh.vertexCount = litCubePrimitive.getVertexCount( );
//    memcpy( &litCubeSubMesh.dataRaw[ 0 ], litCubePrimitive.getData( ).data( ), litCubeSubMesh.dataRaw.size() );

    const std::string &plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
    geometryMap[ plainCubePath ] = { };
    MeshGeometry &plainCube = geometryMap[ plainCubePath ];
    SubMeshGeometry &plainCubeSubMesh = plainCube.subGeometries.emplace_back( );
    plainCubeSubMesh.dataRaw = plainCubePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainCubeSubMesh.vertexCount = plainCubePrimitive.getVertexCount( );
//    memcpy( &plainCubeSubMesh.dataRaw[ 0 ], plainCubePrimitive.getData( ).data( ), plainCubeSubMesh.dataRaw.size() );

    const std::string &plainSquarePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );
    geometryMap[ plainSquarePath ] = { };
    MeshGeometry &plainSquare = geometryMap[ plainSquarePath ];
    SubMeshGeometry &plainSquareSubMesh = plainSquare.subGeometries.emplace_back( );
    plainSquareSubMesh.dataRaw = plainSquarePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainSquareSubMesh.vertexCount = plainSquarePrimitive.getVertexCount( );
//    memcpy( &plainSquareSubMesh.dataRaw[ 0 ], plainSquarePrimitive.getData( ).data( ), plainSquareSubMesh.dataRaw.size() );

    const std::string &plainTrianglePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle );
    geometryMap[ plainTrianglePath ] = { };
    MeshGeometry &plainTriangle = geometryMap[ plainTrianglePath ];
    SubMeshGeometry &plainTriangleSubMesh = plainTriangle.subGeometries.emplace_back( );
    plainTriangleSubMesh.dataRaw = plainTrianglePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainTriangleSubMesh.vertexCount = plainTrianglePrimitive.getVertexCount( );
//    memcpy( &plainTriangleSubMesh.dataRaw[ 0 ], plainTrianglePrimitive.getData( ).data( ), plainTriangleSubMesh.dataRaw.size() );
}

std::shared_ptr< ECS::IGameEntity > AssetManager::createEntity( const std::string &meshPath )
{
    std::shared_ptr< ECS::IGameEntity > rootEntity = std::make_shared< ECS::DynamicGameEntity >( );

    loadModel( rootEntity, meshPath );

    return rootEntity;
}

void AssetManager::loadImage( const std::string &path )
{
    int width, height, channels;

    auto filename = PATH( path );
    stbi_uc *contents = stbi_load( filename.c_str( ), &width, &height, &channels, STBI_rgb_alpha );

    if ( contents == nullptr )
    {
        TRACE( "TextureLoader", VERBOSITY_CRITICAL, stbi_failure_reason( ) )

        throw std::runtime_error( "Couldn't find texture." );
    }

    imageMap[ path ] = std::make_shared< SamplerDataAttachment >( );
    auto &ptr = imageMap[ path ];
    ptr->content = contents;
    ptr->width = static_cast< uint32_t >( width );
    ptr->height = static_cast< uint32_t >( height );
    ptr->channels = static_cast< uint32_t >( channels );
}

void AssetManager::loadModel( const std::shared_ptr< ECS::IGameEntity > &rootEntity, const std::string &path )
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    SceneContext context { };
    bool res = loader.LoadASCIIFromFile( &context.model, &err, &warn, path );

    if ( !res )
    {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << err;
        throw std::runtime_error( ss.str( ) );
    }

    for ( const tinygltf::Scene &scene: context.model.scenes )
    {
        // Attach the initial tree structure to the root node because gltf doesn't do it by default
        for ( int node: scene.nodes )
        {
            std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
            rootEntity->addChild( child );

            onEachNode( context, child, path, node );
        }
    }
}

void AssetManager::onEachNode( const SceneContext &context, const std::shared_ptr< ECS::IGameEntity > &currentEntity, const std::string &currentRootPath, const int &currentNode )
{
    const tinygltf::Model &model = context.model;

    for ( int nodeIdx : model.nodes[ currentNode ].children )
    {
        tinygltf::Node node = model.nodes[ nodeIdx ];

        std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
        currentEntity->addChild( child );

        onEachNode( context, child, currentRootPath, nodeIdx );
    }

    tinygltf::Node node = model.nodes[ currentNode ];

    if ( node.mesh == -1 )
    {
        return;
    }

    std::ostringstream keyBuilder;
    keyBuilder << currentRootPath << "#" << node.name;

    currentEntity->createComponent< ECS::CMaterial >( );

    currentEntity->createComponent< ECS::CMesh >( );
    currentEntity->getComponent< ECS::CMesh >( )->path = keyBuilder.str( );

    MeshContext meshContext = { };

    meshContext.geometry = geometryMap[ keyBuilder.str( ) ] = { };
    meshContext.currentEntity = currentEntity;

    onEachMesh( context, meshContext, node.mesh );

    onEachSkin( context, meshContext, node.skin );
}

void AssetManager::onEachMesh( const SceneContext &context, MeshContext meshContext, const int &meshIdx )
{
    const tinygltf::Model &model = context.model;

    MeshGeometry &geometry = meshContext.geometry;

    tinygltf::Mesh mesh = model.meshes[ meshIdx ];

    for ( const tinygltf::Primitive &primitive : mesh.primitives )
    {
        SubMeshGeometry &subMeshGeometry = geometry.subGeometries.emplace_back( );

        tinygltf::Accessor positionAccessor = model.accessors[ primitive.indices ];
        tinygltf::BufferView bufferView = model.bufferViews[ positionAccessor.bufferView ];
        tinygltf::Buffer buffer = model.buffers[ bufferView.buffer ];

        copyAccessorToVectorTransformed(
                subMeshGeometry.vertices, model, tryGetPrimitiveAttribute( primitive, "POSITION" )
        );

        copyAccessorToVectorTransformed(
                subMeshGeometry.normals, model, tryGetPrimitiveAttribute( primitive, "NORMAL" )
        );

        copyAccessorToVectorTransformed(
                subMeshGeometry.textureCoordinates, model, tryGetPrimitiveAttribute( primitive, "TEXCOORD_0" )
        );

        copyAccessorToVectorTransformed(
                subMeshGeometry.indices, model, primitive.indices
        );

        copyAccessorToVectorTransformed(
                subMeshGeometry.boneIndices, model, tryGetPrimitiveAttribute( primitive, "JOINTS_0" )
        );

        copyAccessorToVectorTransformed(
                subMeshGeometry.boneWeights, model, tryGetPrimitiveAttribute( primitive, "WEIGHTS_0" )
        );

        subMeshGeometry.vertexCount = positionAccessor.count;
        subMeshGeometry.drawMode = primitive.mode == 0 ? PrimitiveDrawMode::Point : PrimitiveDrawMode::Triangle;

        packSubGeometry( subMeshGeometry );
    }
}

void AssetManager::onEachSkin( const SceneContext &context, MeshContext meshContext, const int &skinIdx )
{
    const tinygltf::Model &model = context.model;

    tinygltf::Skin skin = model.skins[ skinIdx ];

    std::vector< float > inverseBindMatricesFlat;

    copyAccessorToVectorTransformed( inverseBindMatricesFlat, context.model, skin.inverseBindMatrices );

    std::vector< glm::mat4 > inverseBindMatrices;

    for ( int i = 0; !skin.joints.empty(); ++i )
    {
        inverseBindMatrices.push_back( flatMatToGLMMat( inverseBindMatricesFlat, i * 16 ) );
    }


    for ( int jointIdx: skin.joints )
    {
        onEachJoint( context, meshContext, -1, jointIdx );
    }
}

void AssetManager::onEachJoint( const SceneContext &context, MeshContext meshContext, const std::vector< glm::mat4 >& inverseBindMatrices, const int &parentIdx, const int &jointIdx )
{
    const tinygltf::Model &model = context.model;

    MeshGeometry &geometry = meshContext.geometry;

    tinygltf::Node joint = model.nodes[ jointIdx ];

    MeshJoint meshJoint = { };
    meshJoint.translation = glm::vec3( joint.translation[ 0 ], joint.translation[ 1 ], joint.translation[ 2 ] );
    meshJoint.rotation = glm::vec3( joint.rotation[ 0 ], joint.rotation[ 1 ], joint.rotation[ 2 ] );
    meshJoint.scale = glm::vec3( joint.scale[ 0 ], joint.scale[ 1 ], joint.scale[ 2 ] );
    meshJoint.inverseBindMatrix = inverseBindMatrices[ jointIdx ];

    if ( parentIdx == -1 )
    {
        geometry.jointTree.addNode( jointIdx, meshJoint );
    } else
    {
        geometry.jointTree.addNode( geometry.jointTree.findNode( parentIdx ), jointIdx, meshJoint );
    }

    for ( int childIdx: joint.children )
    {
        onEachJoint( context, meshContext, inverseBindMatrices, jointIdx, childIdx );
    }
}

void AssetManager::packSubGeometry( SubMeshGeometry &geometry )
{

    for ( int i = 0, texIdx = 0, boneIdx = 0; i < geometry.vertexCount * 3; i += 3, texIdx += 2, boneIdx += 4 )
    {
        geometry.dataRaw.push_back( geometry.vertices[ i ] );
        geometry.dataRaw.push_back( geometry.vertices[ i + 1 ] );
        geometry.dataRaw.push_back( geometry.vertices[ i + 2 ] );

        if ( !geometry.normals.empty( ) )
        {
            geometry.dataRaw.push_back( geometry.normals[ i ] );
            geometry.dataRaw.push_back( geometry.normals[ i + 1 ] );
            geometry.dataRaw.push_back( geometry.normals[ i + 2 ] );
        }

        if ( !geometry.textureCoordinates.empty( ) )
        {
            geometry.dataRaw.push_back( geometry.textureCoordinates[ texIdx ] );
            geometry.dataRaw.push_back( geometry.textureCoordinates[ texIdx + 1 ] );
        }

        if ( !geometry.boneIndices.empty( ) )
        {
            geometry.dataRaw.push_back( ( float ) geometry.boneIndices[ boneIdx ] );
            geometry.dataRaw.push_back( ( float ) geometry.boneIndices[ boneIdx + 1 ] );
            geometry.dataRaw.push_back( ( float ) geometry.boneIndices[ boneIdx + 2 ] );
            geometry.dataRaw.push_back( ( float ) geometry.boneIndices[ boneIdx + 3 ] );
        }

        if ( !geometry.boneWeights.empty( ) )
        {
            geometry.dataRaw.push_back( geometry.boneWeights[ boneIdx ] );
            geometry.dataRaw.push_back( geometry.boneWeights[ boneIdx + 1 ] );
            geometry.dataRaw.push_back( geometry.boneWeights[ boneIdx + 2 ] );
            geometry.dataRaw.push_back( geometry.boneWeights[ boneIdx + 3 ] );
        }
    }
}

const MeshGeometry &AssetManager::getMeshGeometry( const std::string &path )
{
    auto find = geometryMap.find( path );

    if ( find == geometryMap.end( ) )
    {
        throw GraphicsException { "AssetManager", "Could not find geometry!" };
    }

    return find->second;
}

std::shared_ptr< SamplerDataAttachment > AssetManager::getImage( const std::string &path )
{
    auto find = imageMap.find( path );

    if ( find == imageMap.end( ) )
    {
        loadImage( path );
        find = imageMap.find( path );
    }

    return find->second;
}

int AssetManager::tryGetPrimitiveAttribute( const tinygltf::Primitive &primitive, const std::string &attribute )
{
    auto attributeSearch = primitive.attributes.find( attribute );

    if ( attributeSearch != primitive.attributes.end( ) )
    {
        return attributeSearch->second;
    }

    return -1;
}

glm::mat4 AssetManager::flatMatToGLMMat( const std::vector< float > &matFlat, int offset )
{
    glm::mat4 mat;

    int column = 0;
    int row = 0;

    for ( int i = 0; i < 0 + 16; ++i )
    {
        mat[ column ][ row ] = matFlat[ offset + i ];

        if ( ++row == 4 )
        {
            column++;
            row = 0;
        }
    }
    return mat;
}

AssetManager::~AssetManager( )
{
    for ( auto &imagePairs: imageMap )
    {
        stbi_image_free( imagePairs.second->content );
    }

}

END_NAMESPACES