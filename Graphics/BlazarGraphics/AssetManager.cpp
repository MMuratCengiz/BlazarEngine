#include "AssetManager.h"
#include "GraphicsException.h"
#include <glm/gtx/quaternion.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

AssetManager::AssetManager( )
{
    const std::string &litCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube );
    geometryMap[ litCubePath ] = { };
    MeshGeometry &litCube = geometryMap[ litCubePath ];
    litCube.vertices = litCubePrimitive.getVertices( );
    litCube.vertexCount = litCubePrimitive.getVertexCount( );

    const std::string &plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
    geometryMap[ plainCubePath ] = { };
    MeshGeometry &plainCube = geometryMap[ plainCubePath ];
    plainCube.vertices = plainCubePrimitive.getVertices( );
    plainCube.vertexCount = plainCubePrimitive.getVertexCount( );

    const std::string &plainSquarePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );
    geometryMap[ plainSquarePath ] = { };
    MeshGeometry &plainSquare = geometryMap[ plainSquarePath ];
    plainSquare.vertices = plainSquarePrimitive.getVertices( );
    plainSquare.vertexCount = plainSquarePrimitive.getVertexCount( );

    const std::string &plainTrianglePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle );
    geometryMap[ plainTrianglePath ] = { };
    MeshGeometry &plainTriangle = geometryMap[ plainTrianglePath ];
    plainTriangle.vertices = plainTrianglePrimitive.getVertices( );
    plainTriangle.vertexCount = plainTrianglePrimitive.getVertexCount( );
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
    const aiScene *scene = importer.ReadFile( path,
                                              aiProcess_Triangulate |
                                              aiProcess_FlipUVs |
                                              aiProcess_GenSmoothNormals |
                                              aiProcess_JoinIdenticalVertices );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
    {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << importer.GetErrorString( );
        throw std::runtime_error( ss.str( ) );
    }

    SceneContext context { };
    context.scene = scene;

    if ( scene->mRootNode->mNumChildren > 0 || scene->mRootNode->mNumMeshes > 0 )
    {
        onEachNode( context, rootEntity, path, scene, scene->mRootNode );
    }

    for ( int animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex )
    {
        auto anim = scene->mAnimations[ animationIndex ];

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
    }
}

void AssetManager::onEachNode( const SceneContext &context,
                               const std::shared_ptr< ECS::IGameEntity > &currentEntity,
                               const std::string &currentRootPath,
                               const aiScene *scene,
                               const aiNode *pNode )
{
    for ( unsigned int i = 0; i < pNode->mNumChildren; ++i )
    {
        if ( pNode->mChildren[ i ]->mNumChildren > 0 || pNode->mChildren[ i ]->mNumMeshes > 0 )
        {
            std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
            currentEntity->addChild( child );

            onEachNode( context, child, currentRootPath, scene, pNode->mChildren[ i ] );
        }
    }

    for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ )
    {
        const aiMesh *mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];

        std::ostringstream keyBuilder;
        keyBuilder << currentRootPath << "#" << mesh->mName.C_Str( );

        std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
        currentEntity->createComponent< ECS::CMaterial >( );

        // Todo implement:
        // child->getComponent< ECS::CMaterial >()->textures.emplace_back( defaultTexture );

        currentEntity->createComponent< ECS::CMesh >( );
        currentEntity->getComponent< ECS::CMesh >( )->path = keyBuilder.str( );
        geometryMap[ keyBuilder.str( ) ] = { };

        onEachMesh( context, currentEntity->getComponent< ECS::CMesh >( ), mesh );
    }
}

void AssetManager::onEachMesh( const SceneContext &context, const std::shared_ptr< ECS::CMesh > &meshComponent, const aiMesh *mesh )
{
    MeshGeometry &geometry = geometryMap[ meshComponent->path ];

    geometry.vertexCount = mesh->mNumVertices * 3;

    fillGeometryBoneData( context, geometry, mesh, nullptr );
    fillGeometryVertexData( context, geometry, mesh, nullptr );

    for ( unsigned int f = 0; f < mesh->mNumFaces; f++ )
    {
        const aiFace &face = mesh->mFaces[ f ];

        for ( unsigned int j = 0; j < face.mNumIndices; j++ )
        {
            geometry.indices.push_back( face.mIndices[ j ] );
        }
    }

    geometry.hasIndices = !geometry.indices.empty( );
}

void AssetManager::fillGeometryVertexData( const SceneContext &context, MeshGeometry &geometry, const aiMesh *mesh, const aiAnimMesh *animMesh )
{
    aiVector3D *vertices = mesh == nullptr ? animMesh->mVertices : mesh->mVertices;
    aiVector3D *normals = mesh == nullptr ? animMesh->mNormals : mesh->mNormals;

    uint32_t numVertices = mesh == nullptr ? animMesh->mNumVertices : mesh->mNumVertices;

    geometry.hasColors = mesh->mMaterialIndex == 0;
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

/*        if ( colors != nullptr )
        {
            auto vecColor = colors[ i ];
            if ( vecColor != nullptr )
            {
                geometry.colors.push_back( vecColor->r );
                geometry.colors.push_back( vecColor->g );
                geometry.colors.push_back( vecColor->b );
                geometry.colors.push_back( vecColor->a );
            }
        }*/

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

        auto *coordinates = mesh == nullptr ? animMesh->mTextureCoords : mesh->mTextureCoords;

        if ( coordinates[ 0 ] )
        {
            const aiVector3D &textureCoordinates = coordinates[ 0 ][ i ];

            geometry.vertices.push_back( textureCoordinates.x );
            geometry.vertices.push_back( textureCoordinates.y );
        }
    }
}

void AssetManager::fillGeometryBoneData( const SceneContext &context, MeshGeometry &geometry, const aiMesh *pMesh, void *pVoid )
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

glm::mat4 AssetManager::aiMatToGLMMat( const aiMatrix4x4 &aiMat )
{
    glm::mat4 mat;

    /*0: */ mat[ 0 ][ 0 ] = aiMat[ 0 ][ 0 ]; /*1:*/ mat[ 0 ][ 0 ] = aiMat[ 0 ][ 1 ]; /*2:*/ mat[ 0 ][ 2 ] = aiMat[ 0 ][ 2 ];/*3:*/ mat[ 0 ][ 0 ] = aiMat[ 0 ][ 3 ];
    /*0: */ mat[ 1 ][ 0 ] = aiMat[ 1 ][ 0 ]; /*1:*/ mat[ 1 ][ 0 ] = aiMat[ 1 ][ 1 ]; /*2:*/ mat[ 1 ][ 2 ] = aiMat[ 1 ][ 2 ];/*3:*/ mat[ 1 ][ 0 ] = aiMat[ 1 ][ 3 ];
    /*0: */ mat[ 2 ][ 0 ] = aiMat[ 2 ][ 0 ]; /*1:*/ mat[ 2 ][ 0 ] = aiMat[ 2 ][ 1 ]; /*2:*/ mat[ 2 ][ 2 ] = aiMat[ 2 ][ 2 ];/*3:*/ mat[ 2 ][ 0 ] = aiMat[ 2 ][ 3 ];
    /*0: */ mat[ 3 ][ 0 ] = aiMat[ 3 ][ 0 ]; /*1:*/ mat[ 3 ][ 0 ] = aiMat[ 3 ][ 1 ]; /*2:*/ mat[ 3 ][ 2 ] = aiMat[ 3 ][ 2 ];/*3:*/ mat[ 3 ][ 0 ] = aiMat[ 3 ][ 3 ];

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