#include "AssetManager.h"
#include "GraphicsException.h"
#include <glm/gtx/quaternion.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

AssetManager::AssetManager( )
{
    const std::string &litCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube );
    geometryMap[ litCubePath ] = { };
    MeshGeometry &litCube = geometryMap[ litCubePath ];
    SubMeshGeometry &litCubeSubMesh = litCube.subGeometries.emplace_back( );
    litCubeSubMesh.dataRaw = litCubePrimitive.getData(); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    litCubeSubMesh.vertexCount = litCubePrimitive.getVertexCount( );
//    memcpy( &litCubeSubMesh.dataRaw[ 0 ], litCubePrimitive.getData( ).data( ), litCubeSubMesh.dataRaw.size() );

    const std::string &plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
    geometryMap[ plainCubePath ] = { };
    MeshGeometry &plainCube = geometryMap[ plainCubePath ];
    SubMeshGeometry &plainCubeSubMesh = plainCube.subGeometries.emplace_back( );
    plainCubeSubMesh.dataRaw = plainCubePrimitive.getData(); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainCubeSubMesh.vertexCount = plainCubePrimitive.getVertexCount( );
//    memcpy( &plainCubeSubMesh.dataRaw[ 0 ], plainCubePrimitive.getData( ).data( ), plainCubeSubMesh.dataRaw.size() );

    const std::string &plainSquarePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );
    geometryMap[ plainSquarePath ] = { };
    MeshGeometry &plainSquare = geometryMap[ plainSquarePath ];
    SubMeshGeometry &plainSquareSubMesh = plainSquare.subGeometries.emplace_back( );
    plainSquareSubMesh.dataRaw = plainSquarePrimitive.getData(); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
    plainSquareSubMesh.vertexCount = plainSquarePrimitive.getVertexCount( );
//    memcpy( &plainSquareSubMesh.dataRaw[ 0 ], plainSquarePrimitive.getData( ).data( ), plainSquareSubMesh.dataRaw.size() );

    const std::string &plainTrianglePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle );
    geometryMap[ plainTrianglePath ] = { };
    MeshGeometry &plainTriangle = geometryMap[ plainTrianglePath ];
    SubMeshGeometry &plainTriangleSubMesh = plainTriangle.subGeometries.emplace_back( );
    plainTriangleSubMesh.dataRaw = plainTrianglePrimitive.getData(); //.resize( litCubePrimitive.getVertexCount( ) * sizeof ( float ) );
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

            onEachScene( context, child, path, context.model, node );
        }
    }
/*
    for ( int animationIndex = 0; animationIndex < sceneAssimp->mNumAnimations; ++animationIndex )
    {
        auto anim = sceneAssimp->mAnimations[ animationIndex ];

        auto animName = std::string( anim->mName.data );

        for ( int channelIndex = 0; channelIndex < anim->mNumChannels; ++channelIndex )
        {
            aiNodeAnim *animNode = anim->mChannels[ channelIndex ];

            MeshGeometry &geometry = geometryMap[ std::string( animNode->mNodeName.data ) ];

            geometry.animations[ animName ] = { };
            AnimationData &animData = geometry.animations[ animName ];
            animData.duration = anim->mDuration;
            animData.ticksPerSeconds = anim->mTicksPerSecond;

            for ( int transformationIndex = 0; transformationIndex < animNode->mNumPositionKeys; ++transformationIndex )
            {
                auto &animTransformation = animData.boneTransformations.emplace_back( );

                auto aiTranslation = animNode->mPositionKeys[ transformationIndex ];
                auto aiRotation = animNode->mRotationKeys[ transformationIndex ];
                auto aiScale = animNode->mScalingKeys[ transformationIndex ];

                glm::vec3 translation( aiTranslation.mValue.x, aiTranslation.mValue.y, aiTranslation.mValue.z );
                glm::quat rotation( aiRotation.mValue.x, aiRotation.mValue.y, aiRotation.mValue.z, aiRotation.mValue.w );
                glm::vec3 scale( aiScale.mValue.x, aiScale.mValue.y, aiScale.mValue.z );

                glm::mat4 modelMatrix { 1 };

                modelMatrix = glm::translate( modelMatrix, translation );
                modelMatrix = glm::scale( modelMatrix, scale );
                modelMatrix *= glm::mat4_cast( rotation );

                animData.boneTransformations.push_back( modelMatrix );
            }
        }
    }*/
}

void AssetManager::onEachScene( const SceneContext &context, const std::shared_ptr< ECS::IGameEntity > &currentEntity, const std::string &currentRootPath, const tinygltf::Model &model, const int& currentNode )
{
    for ( int nodeIdx : model.nodes[ currentNode ].children )
    {
        tinygltf::Node node = model.nodes[ nodeIdx ];

        if ( !node.children.empty() )
        {
            std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
            currentEntity->addChild( child );

            onEachScene( context, child, currentRootPath, model, nodeIdx );
        }

    }

    tinygltf::Node node = model.nodes[ currentNode ];

    if ( node.mesh == -1 )
    {
        return;
    }

    tinygltf::Mesh mesh = model.meshes[ node.mesh ];

    std::ostringstream keyBuilder;
    keyBuilder << currentRootPath << "#" << mesh.name;

    currentEntity->createComponent< ECS::CMaterial >( );

    // Todo implement:
    // child->getComponent< ECS::CMaterial >()->textures.emplace_back( defaultTexture );

    currentEntity->createComponent< ECS::CMesh >( );
    currentEntity->getComponent< ECS::CMesh >( )->path = keyBuilder.str( );
    geometryMap[ keyBuilder.str( ) ] = { };

    onEachMesh( context, currentEntity->getComponent< ECS::CMesh >( ), model, node.mesh );
}

void AssetManager::onEachMesh( const SceneContext &context, const std::shared_ptr< ECS::CMesh > &meshComponent, const tinygltf::Model& model, const int& meshIdx )
{
    MeshGeometry &geometry = geometryMap[ meshComponent->path ];

    tinygltf::Mesh mesh = model.meshes[ meshIdx ];

    auto tryGetAttribute = [ ]( const tinygltf::Primitive& primitive, const std::string & attribute ) -> int
    {
        auto attributeSearch = primitive.attributes.find( attribute );

        if ( attributeSearch != primitive.attributes.end() )
        {
            return attributeSearch->second;
        }

        // todo support morph targets

        return -1;
    };

    for ( const tinygltf::Primitive& primitive : mesh.primitives )
    {
        SubMeshGeometry & subMeshGeometry = geometry.subGeometries.emplace_back( );

        tinygltf::Accessor positionAccessor = model.accessors[ primitive.indices ];
        tinygltf::BufferView bufferView = model.bufferViews[ positionAccessor.bufferView ];
        tinygltf::Buffer buffer = model.buffers[ bufferView.buffer ];

        copyAccessorToVector(
                subMeshGeometry.vertices, model, tryGetAttribute( primitive, "POSITION" )
        );

        copyAccessorToVector(
                subMeshGeometry.normals, model, tryGetAttribute( primitive, "NORMAL" )
        );

        copyAccessorToVector(
                subMeshGeometry.textureCoordinates, model, tryGetAttribute( primitive, "TEXCOORD_0" )
        );

        copyAccessorToVector(
                subMeshGeometry.indices, model, primitive.indices
        );

        subMeshGeometry.vertexCount = subMeshGeometry.vertices.size( );
        subMeshGeometry.drawMode =  primitive.mode == 0 ? PrimitiveDrawMode::Point : PrimitiveDrawMode::Triangle;

        packSubGeometry( subMeshGeometry );
    }

    int x = 1;
}

/*
void AssetManager::fillGeometryVertexData( const SceneContext &context, MeshGeometry &geometry, const aiMesh *mesh, const aiAnimMesh *animMesh )
{
    aiVector3D *vertices = mesh == nullptr ? animMesh->mVertices : mesh->mVertices;
    aiVector3D *normals = mesh == nullptr ? animMesh->mNormals : mesh->mNormals;

    uint32_t numVertices = mesh == nullptr ? animMesh->mNumVertices : mesh->mNumVertices;

    geometry.hasColors = mesh != nullptr && mesh->mMaterialIndex == 0;
    geometry.hasBoneData = !geometry.boneIndices.empty( );

    glm::vec3 currentNormal;

    for ( uint32_t i = 0; i < numVertices; ++i )
    {
        const auto &vec = vertices[ i ];

        geometry.vertices.push_back( vec.x );
        geometry.vertices.push_back( vec.y );
        geometry.vertices.push_back( vec.z );

        const auto &normal = normals[ i ];

        geometry.vertices.push_back( normal.x );
        geometry.vertices.push_back( normal.y );
        geometry.vertices.push_back( normal.z );

*//*        if ( colors != nullptr )
        {
            auto vecColor = colors[ i ];
            if ( vecColor != nullptr )
            {
                geometry.colors.push_back( vecColor->r );
                geometry.colors.push_back( vecColor->g );
                geometry.colors.push_back( vecColor->b );
                geometry.colors.push_back( vecColor->a );
            }
        }*//*

        auto *coordinates = mesh == nullptr ? animMesh->mTextureCoords : mesh->mTextureCoords;

        if ( coordinates[ 0 ] )
        {
            const aiVector3D &textureCoordinates = coordinates[ 0 ][ i ];

            geometry.vertices.push_back( textureCoordinates.x );
            geometry.vertices.push_back( textureCoordinates.y );
        }

        if ( !geometry.boneIndices.empty( ) )
        {
            geometry.vertices.push_back( geometry.boneIndices[ i ] );
            geometry.vertices.push_back( geometry.boneIndices[ i + 1 ] );
            geometry.vertices.push_back( geometry.boneIndices[ i + 2 ] );
            geometry.vertices.push_back( geometry.boneIndices[ i + 3 ] );

            geometry.vertices.push_back( geometry.boneWeights[ i ] );
            geometry.vertices.push_back( geometry.boneWeights[ i + 1 ] );
            geometry.vertices.push_back( geometry.boneWeights[ i + 2 ] );
            geometry.vertices.push_back( geometry.boneWeights[ i + 3 ] );
        }
    }
}*/

void AssetManager::packSubGeometry( SubMeshGeometry &geometry )
{
    for ( int i = 0, texIdx = 0; i < geometry.vertexCount; i += 3, texIdx += 2 )
    {
        geometry.dataRaw.push_back( geometry.vertices[ i ] );
        geometry.dataRaw.push_back( geometry.vertices[ i + 1 ] );
        geometry.dataRaw.push_back( geometry.vertices[ i + 2 ] );

        if ( !geometry.normals.empty() )
        {
            geometry.dataRaw.push_back( geometry.normals[ i ] );
            geometry.dataRaw.push_back( geometry.normals[ i + 1 ] );
            geometry.dataRaw.push_back( geometry.normals[ i + 2 ] );
        }

        if ( !geometry.textureCoordinates.empty() )
        {
            geometry.dataRaw.push_back( geometry.textureCoordinates[ texIdx ] );
            geometry.dataRaw.push_back( geometry.textureCoordinates[ texIdx + 1 ] );
        }
    }
}

/*void AssetManager::fillGeometryBoneData( const SceneContext &context, MeshGeometry &geometry, const aiMesh *pMesh, void *pVoid )
{
    FUNCTION_BREAK( pMesh->mNumBones == 0 )

    geometry.boneIndices.resize( pMesh->mNumVertices * SUPPORTED_BONE_COUNT, -1 );
    geometry.boneWeights.resize( pMesh->mNumVertices * SUPPORTED_BONE_COUNT, -1 );

    for ( int i = 0; i < pMesh->mNumBones; ++i )
    {
        const aiBone *bone = pMesh->mBones[ i ];

        geometry.boneOffsetMatrices.push_back( aiMatToGLMMat( bone->mOffsetMatrix ) );

        for ( int j = 0; j < bone->mNumWeights; ++j )
        {
            auto weight = bone->mWeights[ j ];

            for ( uint32_t offset = weight.mVertexId; offset < weight.mVertexId + 4; ++offset )
            {
                if ( geometry.boneIndices[ offset ] == -1 )
                {
                    geometry.boneIndices[ offset ] = i;
                    geometry.boneWeights[ offset ] = weight.mWeight;
                }
            }
        }
    }
}*/

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

/*glm::mat4 AssetManager::aiMatToGLMMat( const aiMatrix4x4 &aiMat )
{
    glm::mat4 mat;

    *//*0: *//* mat[ 0 ][ 0 ] = aiMat[ 0 ][ 0 ]; *//*1:*//* mat[ 0 ][ 0 ] = aiMat[ 0 ][ 1 ]; *//*2:*//* mat[ 0 ][ 2 ] = aiMat[ 0 ][ 2 ];*//*3:*//* mat[ 0 ][ 0 ] = aiMat[ 0 ][ 3 ];
    *//*0: *//* mat[ 1 ][ 0 ] = aiMat[ 1 ][ 0 ]; *//*1:*//* mat[ 1 ][ 0 ] = aiMat[ 1 ][ 1 ]; *//*2:*//* mat[ 1 ][ 2 ] = aiMat[ 1 ][ 2 ];*//*3:*//* mat[ 1 ][ 0 ] = aiMat[ 1 ][ 3 ];
    *//*0: *//* mat[ 2 ][ 0 ] = aiMat[ 2 ][ 0 ]; *//*1:*//* mat[ 2 ][ 0 ] = aiMat[ 2 ][ 1 ]; *//*2:*//* mat[ 2 ][ 2 ] = aiMat[ 2 ][ 2 ];*//*3:*//* mat[ 2 ][ 0 ] = aiMat[ 2 ][ 3 ];
    *//*0: *//* mat[ 3 ][ 0 ] = aiMat[ 3 ][ 0 ]; *//*1:*//* mat[ 3 ][ 0 ] = aiMat[ 3 ][ 1 ]; *//*2:*//* mat[ 3 ][ 2 ] = aiMat[ 3 ][ 2 ];*//*3:*//* mat[ 3 ][ 0 ] = aiMat[ 3 ][ 3 ];

    return mat;
}*/

AssetManager::~AssetManager( )
{
    for ( auto &imagePairs: imageMap )
    {
        stbi_image_free( imagePairs.second->content );
    }

}

END_NAMESPACES