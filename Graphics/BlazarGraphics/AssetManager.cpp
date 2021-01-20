#include "AssetManager.h"
#include "GraphicsException.h"

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
    auto & ptr = imageMap[ path ];
    ptr->content = contents;
    ptr->width = static_cast< uint32_t >( width );
    ptr->height = static_cast< uint32_t >( height );
    ptr->channels = static_cast< uint32_t >( channels );
}

void AssetManager::loadModel( const std::shared_ptr< ECS::IGameEntity >& rootEntity, const std::string &path )
{
    const aiScene *scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
    {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << importer.GetErrorString( );
        throw std::runtime_error( ss.str( ) );
    }

    if ( scene->mRootNode->mNumChildren > 0 || scene->mRootNode->mNumMeshes > 0 )
    {
        onEachNode( rootEntity, path, scene, scene->mRootNode );
    }
    // TODO handle scene->mAnimations
}

void AssetManager::onEachNode( const std::shared_ptr< ECS::IGameEntity >& currentEntity, const std::string& currentRootPath, const aiScene *scene, const aiNode *pNode )
{
    auto metadata = pNode->mMetaData;

    // Todo test child mechanism
    for ( unsigned int i = 0; i < pNode->mNumChildren; ++i )
    {
        if ( pNode->mChildren[ i ]->mNumChildren > 0 || pNode->mChildren[ i ]->mNumMeshes > 0 )
        {
            std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
            currentEntity->addChild( child );

            onEachNode( child, currentRootPath, scene, pNode->mChildren[ i ] );
        }
    }

    for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ )
    {
        const aiMesh *mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];

        std::ostringstream keyBuilder;
        keyBuilder << currentRootPath << "#" << mesh->mName.C_Str();

        std::shared_ptr< ECS::IGameEntity > child = std::make_shared< ECS::DynamicGameEntity >( );
        currentEntity->createComponent< ECS::CMaterial >( );

        // Todo implement:
        // child->getComponent< ECS::CMaterial >()->textures.emplace_back( defaultTexture );

        currentEntity->createComponent< ECS::CMesh >( );
        currentEntity->getComponent< ECS::CMesh >( )->path = keyBuilder.str( );
        geometryMap[ keyBuilder.str( ) ] = { };

        onEachMesh( currentEntity->getComponent< ECS::CMesh >( ), mesh );
    }
}

void AssetManager::onEachMesh( const std::shared_ptr< ECS::CMesh >& meshComponent, const aiMesh *mesh )
{
    MeshGeometry &geometry = geometryMap[ meshComponent->path ];

    geometry.vertexCount = mesh->mNumVertices * 3;

    fillGeometryVertexData( geometry, mesh, nullptr );

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

void AssetManager::fillGeometryVertexData( MeshGeometry& geometry, const aiMesh *mesh, const aiAnimMesh *animMesh )
{
    aiVector3D *vertices = mesh == nullptr ? animMesh->mVertices : mesh->mVertices;
    aiVector3D *normals = mesh == nullptr ? animMesh->mNormals : mesh->mNormals;
    uint32_t numVertices = mesh == nullptr ? animMesh->mNumVertices : mesh->mNumVertices;


    bool hasNormals = normals != nullptr;

    uint32_t currentTriangleVertexIndex = 0;
    glm::vec3 currentNormal;

    for ( uint32_t i = 0; i < numVertices; ++i )
    {
        const auto &vec = vertices[ i ];

        geometry.vertices.push_back( vec.x );
        geometry.vertices.push_back( vec.y );
        geometry.vertices.push_back( vec.z );

        if ( hasNormals )
        {
            const auto &normal = normals[ i ];

            geometry.vertices.push_back( normal.x );
            geometry.vertices.push_back( normal.y );
            geometry.vertices.push_back( normal.z );
        }
        else
        {
            // todo not accurate
            if ( currentTriangleVertexIndex == 0 && i + currentTriangleVertexIndex + 2 < numVertices )
            {
                const auto &first = vertices[ i + currentTriangleVertexIndex ];
                const auto &second = vertices[ i + currentTriangleVertexIndex + 1 ];
                const auto &third = vertices[ i + currentTriangleVertexIndex + 2 ];

                const auto edge1 = first - second;
                const auto edge2 = third - second;

                currentNormal = glm::cross( glm::vec3( edge1.x, edge1.y, edge1.z ), glm::vec3( edge2.x, edge2.y, edge2.z ) );
                currentNormal = glm::normalize( currentNormal );
            }

            geometry.vertices.push_back( currentNormal.x );
            geometry.vertices.push_back( currentNormal.y );
            geometry.vertices.push_back( currentNormal.z );

            currentTriangleVertexIndex++;

            if ( currentTriangleVertexIndex >= 3 )
            {
                currentTriangleVertexIndex = 0;
            }
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

const MeshGeometry &AssetManager::getMeshGeometry( const std::string &path )
{
    auto find = geometryMap.find( path );

    if ( find == geometryMap.end( ) )
    {
        throw GraphicsException{ "AssetManager", "Could not find geometry!" };
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

AssetManager::~AssetManager( )
{
    for ( auto& imagePairs: imageMap )
    {
        stbi_image_free( imagePairs.second->content );
    }

}

END_NAMESPACES