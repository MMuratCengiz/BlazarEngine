#include "MeshLoader.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

MeshLoader::MeshLoader( std::shared_ptr< InstanceContext > context, pCommandExecutor &commandExecutor ) : context( std::move( context ) ), commandExecutor( commandExecutor )
{
    const std::string &lightedCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube );
    loadedModels[ lightedCubePath ] = { };
    ObjectBufferList &lightedCubeBufferList = loadedModels[ lightedCubePath ];
    ObjectBuffer &lightedCubeBuffer = lightedCubeBufferList.buffers.emplace_back( );

    copyVertexBuffer( lightedCubeBuffer.vertexBuffer, lightedCubePrimitive.getVertexBuffer( ) );
    lightedCubeBuffer.vertexCount = lightedCubePrimitive.getVertexCount( );

    const std::string &plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
    loadedModels[ plainCubePath ] = { };
    ObjectBufferList &plainCubeBufferList = loadedModels[ plainCubePath ];
    ObjectBuffer &plainCubeBuffer = plainCubeBufferList.buffers.emplace_back( );

    copyVertexBuffer( plainCubeBuffer.vertexBuffer, plainCubePrimitive.getVertexBuffer( ) );
    plainCubeBuffer.vertexCount = plainCubePrimitive.getVertexCount( );
}

void MeshLoader::cache( const ECS::CMesh &mesh )
{
    if ( loadedModels.find( mesh.path ) == loadedModels.end( ) )
    {
        loadModel( mesh );
    }
}


void MeshLoader::load( ObjectBufferList &objectBuffer, const ECS::CMesh &mesh )
{
    cache( mesh );
    objectBuffer = loadedModels[ mesh.path ];
}

void MeshLoader::loadModel( const ECS::CMesh &mesh )
{
    const aiScene *scene = importer.ReadFile( mesh.path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
    {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << importer.GetErrorString( );
        throw std::runtime_error( ss.str( ) );
    }

    loadedModels[ mesh.path ] = ObjectBufferList { };
    onEachNode( loadedModels[ mesh.path ], scene, scene->mRootNode );
}

void MeshLoader::onEachNode( ObjectBufferList &buffer, const aiScene *scene, const aiNode *pNode )
{
    auto metadata = pNode->mMetaData;

    for ( unsigned int i = 0; i < pNode->mNumChildren; ++i )
    {
        onEachNode( buffer, scene, pNode->mChildren[ i ] );
    }

    for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ )
    {
        const aiMesh *mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];
        onEachMesh( buffer, mesh );
    }
}

void MeshLoader::onEachMesh( ObjectBufferList &buffer, const aiMesh *mesh )
{
    ObjectBuffer &bufferPart = buffer.buffers.emplace_back( ObjectBuffer { } );

    bufferPart.vertexCount = mesh->mNumVertices * 3;

    for ( uint32_t animIndex = 0; animIndex < mesh->mNumAnimMeshes; ++animIndex )
    {
        onEachAnimMesh( bufferPart, mesh->mAnimMeshes[ animIndex ] );
    }

    Core::DynamicMemory memory = getMeshMemory( mesh, nullptr );

    for ( unsigned int f = 0; f < mesh->mNumFaces; f++ )
    {
        const aiFace &face = mesh->mFaces[ f ];

        for ( unsigned int j = 0; j < face.mNumIndices; j++ )
        {
            bufferPart.indices.emplace_back( face.mIndices[ j ] );
        }
    }

    bufferPart.indexCount = bufferPart.indices.size( );

    copyVertexBuffer( bufferPart.vertexBuffer, memory );

    if ( !bufferPart.indices.empty( ) )
    {
        copyIndexBuffer( bufferPart, bufferPart.indices );
    }
}

void MeshLoader::onEachAnimMesh( ObjectBuffer &buffer, const aiAnimMesh *mesh )
{
    auto &vbo = buffer.animBufferList.emplace_back( );

    auto memory = getMeshMemory( nullptr, mesh );

    copyVertexBuffer( vbo, memory );
}

Core::DynamicMemory MeshLoader::getMeshMemory( const aiMesh *mesh, const aiAnimMesh *animMesh )
{
    aiVector3D *vertices = mesh == nullptr ? animMesh->mVertices : mesh->mVertices;
    aiVector3D *normals = mesh == nullptr ? animMesh->mNormals : mesh->mNormals;
    uint32_t numVertices = mesh == nullptr ? animMesh->mNumVertices : mesh->mNumVertices;

    Core::DynamicMemory memory { numVertices * 8 * sizeof( float ) };

    // Todo generate manuals automatically if not specified.

    bool hasNormals = normals != nullptr;

    uint32_t currentTriangleVertexIndex = 0;
    glm::vec3 currentNormal;

    for ( uint32_t i = 0; i < numVertices; ++i )
    {
        const auto &vec = vertices[ i ];

        memory.attachElements< float >( { vec.x, vec.y, vec.z } );

        if ( hasNormals )
        {
            const auto &normal = normals[ i ];
            memory.attachElements< float >( { normal.x, normal.y, normal.z } );
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

            memory.attachElements< float >( { currentNormal.x, currentNormal.y, currentNormal.z } );
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

            memory.attachElements< float >( { textureCoordinates.x, textureCoordinates.y } );
        }
    }

    return memory;
}

void MeshLoader::copyVertexBuffer( std::pair< vk::Buffer, vma::Allocation > &buffer, const Core::DynamicMemory &memory )
{
    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    RenderUtilities::initStagingBuffer( context, stagingBuffer, memory.data( ), memory.size( ) );

    vk::BufferCreateInfo bufferCreateInfo { };
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    bufferCreateInfo.size = memory.size( );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo { };
    allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    buffer = context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->copyBuffer( memory.size( ), stagingBuffer.first, buffer.first )
            ->execute( );

    context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
}

void MeshLoader::copyIndexBuffer( ObjectBuffer &bufferPart, const std::vector< uint32_t > &indices )
{
    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    RenderUtilities::initStagingBuffer( context, stagingBuffer, indices.data( ), indices.size( ) * sizeof( uint32_t ) );

    vk::BufferCreateInfo bufferCreateInfo { };
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    bufferCreateInfo.size = indices.size( ) * sizeof( uint32_t );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo { };
    allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    bufferPart.indexBuffer = context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->copyBuffer( indices.size( ) * sizeof( uint32_t ), stagingBuffer.first, bufferPart.indexBuffer.first )
            ->execute( );

    context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
}

MeshLoader::~MeshLoader( )
{
    for ( auto &pair: loadedModels )
    {
        for ( auto &buffer: pair.second.buffers )
        {
            for ( auto &animBuffer: buffer.animBufferList )
            {
                context->vma.destroyBuffer( animBuffer.first, animBuffer.second );
            }

            context->vma.destroyBuffer( buffer.vertexBuffer.first, buffer.vertexBuffer.second );
            if ( buffer.indexCount > 0 )
            {
                context->vma.destroyBuffer( buffer.indexBuffer.first, buffer.indexBuffer.second );
            }
        }
    }
}


END_NAMESPACES