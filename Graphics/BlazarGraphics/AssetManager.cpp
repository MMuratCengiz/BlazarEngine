#include "AssetManager.h"
#include "GraphicsException.h"
#include <glm/gtx/quaternion.hpp>
#include <boost/format.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

AssetManager::AssetManager( )
{
    geometryTable.resize( 4 );
    const std::string &litCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube );
    geometryTable[ 0 ] = { };
    MeshGeometry &litCube = geometryTable[ 0 ];
    SubMeshGeometry &litCubeSubMesh = litCube.subGeometries.emplace_back( );
    litCubeSubMesh.dataRaw = litCubePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    litCubeSubMesh.vertexCount = litCubePrimitive.getVertexCount( );

    const std::string &plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
    geometryTable[ 1 ] = { };
    MeshGeometry &plainCube = geometryTable[ 1 ];
    SubMeshGeometry &plainCubeSubMesh = plainCube.subGeometries.emplace_back( );
    plainCubeSubMesh.dataRaw = plainCubePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainCubeSubMesh.vertexCount = plainCubePrimitive.getVertexCount( );

    const std::string &plainSquarePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );
    geometryTable[ 2 ] = { };
    MeshGeometry &plainSquare = geometryTable[ 2 ];
    SubMeshGeometry &plainSquareSubMesh = plainSquare.subGeometries.emplace_back( );
    plainSquareSubMesh.dataRaw = plainSquarePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainSquareSubMesh.vertexCount = plainSquarePrimitive.getVertexCount( );

    const std::string &plainTrianglePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle );
    geometryTable[ 3 ] = { };
    MeshGeometry &plainTriangle = geometryTable[ 3 ];
    SubMeshGeometry &plainTriangleSubMesh = plainTriangle.subGeometries.emplace_back( );
    plainTriangleSubMesh.dataRaw = plainTrianglePrimitive.getData( ); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainTriangleSubMesh.vertexCount = plainTrianglePrimitive.getVertexCount( );
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
    context.rootEntity = rootEntity;

    bool res = loader.LoadASCIIFromFile( &context.model, &err, &warn, path );

    if ( !res )
    {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << err;
        throw std::runtime_error( ss.str( ) );
    }

    generateAnimationData( context );

    for ( const tinygltf::Scene &scene: context.model.scenes )
    {
        // Attach the initial tree structure to the root node because gltf doesn't do it by default
        for ( int node: scene.nodes )
        {
            std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
            rootEntity->addChild( child );

            addNode( context, -1, node );
            onEachNode( context, child, path, node );
        }
    }
}

void AssetManager::generateAnimationData( SceneContext &context )
{
    const tinygltf::Model &model = context.model;

    for ( const tinygltf::Animation &animation: model.animations )
    {
        AnimationData animationData = { };

        for ( const tinygltf::AnimationChannel &channel: animation.channels )
        {
            onEachChannel( model, animation, animationData, channel );
        }

        context.animations[ animation.name ] = std::move( animationData );
    }

}

void AssetManager::onEachChannel( const tinygltf::Model &model, const tinygltf::Animation &animation, AnimationData &animationData, const tinygltf::AnimationChannel &channel )
{
    AnimationChannel animationChannel = { };

    animationChannel.targetJoint = channel.target_node;

    const tinygltf::AnimationSampler &sampler = animation.samplers[ channel.sampler ];

    std::string targetPath = channel.target_path;
    boost::to_lower( targetPath );

    std::string interpolationType = sampler.interpolation;
    boost::to_lower( interpolationType );

    animationChannel.transformType =
            targetPath == "translation" ? Translation :
            targetPath == "rotation" ? Rotation :
            targetPath == "scale" ? Scale :
            Weights;

    animationChannel.interpolationType =
            interpolationType == "linear" ? Linear :
            interpolationType == "step" ? Step :
            CubicSpline;

    std::vector< float > transformFlat;

    copyAccessorToVectorTransformed( animationChannel.keyFrames, model, sampler.input );
    copyAccessorToVectorTransformed( transformFlat, model, sampler.output );

    tinygltf::Accessor accessor = model.accessors[ sampler.output ];

    for ( int i = 0; i < animationChannel.keyFrames.size( ); ++i )
    {
        switch ( accessor.type )
        {
            case TINYGLTF_TYPE_VEC3:
            {
                glm::vec3 data = glm::make_vec3( transformFlat.data( ) + ( i * 3 ) );
                animationChannel.transform.emplace_back( data, 0.0f );
                break;
            }
            case TINYGLTF_TYPE_VEC4:
            {
                glm::vec4 data = glm::make_vec4( transformFlat.data( ) + ( i * 4 ) );
                animationChannel.transform.push_back( data );
                break;
            }
        }
    }

    animationData.channels.push_back( std::move( animationChannel ) );
}

void AssetManager::onEachNode( SceneContext &context, const std::shared_ptr< ECS::IGameEntity > &currentEntity, const std::string &currentRootPath, const int &currentNode )
{
    const tinygltf::Model &model = context.model;

    for ( int nodeIdx : model.nodes[ currentNode ].children )
    {
        tinygltf::Node node = model.nodes[ nodeIdx ];

        addNode( context, currentNode, nodeIdx );

        if ( node.mesh != -1 )
        {
            std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );

            currentEntity->addChild( child );
            onEachNode( context, child, currentRootPath, nodeIdx );
        }
        else
        {
            onEachNode( context, currentEntity, currentRootPath, nodeIdx );
        }
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

    if ( !context.model.animations.empty( ) )
    {
        auto animState = currentEntity->createComponent< ECS::CAnimState >( );
        animState->mesh = currentEntity->getComponent< ECS::CMesh >( );
    }

    MeshContext meshContext = { };
    geometryTable.push_back( { } );

    meshContext.geometryIdx = geometryTable.size( ) - 1;
    meshContext.meshNodeIdx = currentNode;
    meshContext.entity = currentEntity;

    currentEntity->getComponent< ECS::CMesh >( )->path = keyBuilder.str( );
    currentEntity->getComponent< ECS::CMesh >( )->geometryRefIdx = meshContext.geometryIdx;

    onEachMesh( context, meshContext, node.mesh );

    onEachSkin( context, meshContext, node.skin );
}

void AssetManager::onEachMesh( SceneContext &context, MeshContext meshContext, const int &meshIdx )
{
    const tinygltf::Model &model = context.model;

    MeshGeometry &geometry = geometryTable[ meshContext.geometryIdx ];

    geometry.nodeTree = context.nodeTree;
    geometry.meshNodeIdx = meshContext.meshNodeIdx;
    geometry.animations = context.animations;

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

        subMeshGeometry.vertexCount = subMeshGeometry.vertices.size( ) / 3;
        subMeshGeometry.drawMode = primitive.mode == 0 ? PrimitiveDrawMode::Point : PrimitiveDrawMode::Triangle;

        packSubGeometry( subMeshGeometry );
    }
}

void AssetManager::onEachSkin( SceneContext &context, MeshContext meshContext, const int &skinIdx )
{
    const tinygltf::Model &model = context.model;

    tinygltf::Skin skin = model.skins[ skinIdx ];

    std::vector< float > inverseBindMatricesFlat;

    copyAccessorToVectorTransformed( inverseBindMatricesFlat, context.model, skin.inverseBindMatrices );

    MeshGeometry &geometry = geometryTable[ meshContext.geometryIdx ];

    geometry.joints = skin.joints;

    for ( int i = 0; i < skin.joints.size( ); i++ )
    {
        int joint = skin.joints[ i ];

        auto jointNode = geometry.nodeTree.findNode( joint );
        jointNode->data.inverseBindMatrix = flatMatToGLMMat( inverseBindMatricesFlat, i * 16 );
    }
}

void AssetManager::packSubGeometry( SubMeshGeometry &geometry )
{
    auto pushToDataRaw = [ & ]( const std::vector< float > &source, const int &offset, const int &count ) -> void
    {
        if ( source.empty( ) )
        {
            return;
        }

        for ( int i = 0; i < count; ++i )
        {
            geometry.dataRaw.push_back( ( float ) source[ offset + i ] );
        }
    };

    for ( int i = 0, texIdx = 0, boneIdx = 0; i < geometry.vertexCount * 3; i += 3, texIdx += 2, boneIdx += 4 )
    {
        pushToDataRaw( geometry.vertices, i, 3 );
        pushToDataRaw( geometry.normals, i, 3 );
        pushToDataRaw( geometry.textureCoordinates, texIdx, 2 );
        pushToDataRaw( geometry.boneIndices, boneIdx, 4 );
        pushToDataRaw( geometry.boneWeights, boneIdx, 4 );
    }
}

MeshGeometry &AssetManager::getMeshGeometry( const int &geometryIdx, const std::string &builtinPrimitive )
{
    int idx = geometryIdx;

    if ( idx == -1 )
    {
        idx = findBuiltinPrimitiveIdx( builtinPrimitive );
        if ( idx == -1 )
        {
            throw GraphicsException { "AssetManager", "Could not find geometry!" };
        }
    }

    return geometryTable[ idx ];
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

int AssetManager::findBuiltinPrimitiveIdx( const std::string &primitiveName ) const
{
    if ( primitiveName == BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube ) )
    {
        return 0;
    }

    if ( primitiveName == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube ) )
    {
        return 1;
    }

    if ( primitiveName == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare ) )
    {
        return 2;
    }

    if ( primitiveName == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle ) )
    {
        return 3;
    }

    return -1;
}

glm::mat4 AssetManager::flatMatToGLMMat( const std::vector< float > &matFlat, int offset )
{
    return glm::make_mat4( matFlat.data( ) + offset );
}

void AssetManager::addNode( SceneContext &sceneContext, int parent, int nodeId )
{
    tinygltf::Node node = sceneContext.model.nodes[ nodeId ];

    auto nodeTreeRef = sceneContext.nodeTree.findNode( nodeId );

    if ( nodeTreeRef == nullptr || parent == -1 )
    {
        MeshNode meshNode = { };
        meshNode.transform = glm::mat4(1.0f);
        meshNode.translation = glm::vec3( 0.0f );
        meshNode.scale = glm::vec3( 1.0f );
        meshNode.rotation = glm::mat4( 1.0f );

        if ( !node.translation.empty( ) )
        {
            meshNode.translation = glm::vec3( node.translation[ 0 ], node.translation[ 1 ], node.translation[ 2 ] );
        }

        if ( !node.rotation.empty( ) )
        {
            meshNode.rotation = glm::mat4( glm::quat( node.rotation[ 0 ], node.rotation[ 1 ], node.rotation[ 2 ], node.rotation[ 3 ] ) );
        }

        if ( !node.scale.empty( ) )
        {
            meshNode.scale = glm::vec3( node.scale[ 0 ], node.scale[ 1 ], node.scale[ 2 ] );
        }

        if ( !node.matrix.empty( ) )
        {
            meshNode.transform = glm::make_mat4( node.matrix.data( ) );
            meshNode.isMatSet = true;
        }

        if ( parent == -1 )
        {
            sceneContext.nodeTree.setRootData( nodeId, meshNode );
        }
        else
        {
            sceneContext.nodeTree.addNode( sceneContext.nodeTree.findNode( parent ), nodeId, meshNode );
        }
    }
}

AssetManager::~AssetManager( )
{
    for ( auto &imagePairs: imageMap )
    {
        stbi_image_free( imagePairs.second->content );
    }

    for ( auto geometry: geometryTable )
    {
        geometry.nodeTree.freeNode( geometry.nodeTree.getRoot( ) );
    }
}

END_NAMESPACES