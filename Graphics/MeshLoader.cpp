#include "MeshLoader.h"
#include "../Core/DynamicMemory.h"

NAMESPACES( SomeVulkan, Graphics )

MeshLoader::MeshLoader( std::shared_ptr< InstanceContext > context, pCommandExecutor &commandExecutor ) : context( std::move( context ) ), commandExecutor( commandExecutor ) {
    const std::string &lightedCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LightedCube );
    loadedModels[ lightedCubePath ] = { };
    ObjectBufferList& lightedCubeBufferList = loadedModels[ lightedCubePath ];
    ObjectBuffer& lightedCubeBuffer = lightedCubeBufferList.buffers.emplace_back( );

    copyVertexBuffer( lightedCubeBuffer, lightedCubePrimitive.getVertexBuffer() );
    lightedCubeBuffer.vertexCount = lightedCubePrimitive.getVertexCount();

    const std::string &plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
    loadedModels[ plainCubePath ] = { };
    ObjectBufferList& plainCubeBufferList = loadedModels[ plainCubePath ];
    ObjectBuffer& plainCubeBuffer = plainCubeBufferList.buffers.emplace_back( );

    copyVertexBuffer( plainCubeBuffer, plainCubePrimitive.getVertexBuffer() );
    plainCubeBuffer.vertexCount = plainCubePrimitive.getVertexCount();
}

void MeshLoader::cache( const ECS::CMesh &mesh ) {
    if ( loadedModels.find( mesh.path ) == loadedModels.end() ) {
        loadModel( mesh );
    }
}


void MeshLoader::load( ObjectBufferList& objectBuffer, const ECS::CMesh& mesh ) {
    cache( mesh );
    objectBuffer = loadedModels[ mesh.path ];
}

void MeshLoader::loadModel( const SomeVulkan::ECS::CMesh& mesh ) {
    const aiScene* scene = importer.ReadFile( mesh.path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << importer.GetErrorString( );
        throw std::runtime_error( ss.str( ) );
    }

    loadedModels[ mesh.path ] = ObjectBufferList{ };
    onEachNode( loadedModels[ mesh.path ], scene, scene->mRootNode );
}

void MeshLoader::onEachNode( ObjectBufferList& buffer, const aiScene* scene, const aiNode* pNode ) {
    for ( unsigned int i = 0; i < pNode->mNumChildren; ++i ) {
        onEachNode( buffer, scene, pNode->mChildren[ i ] );
    }

    for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ ) {
        const aiMesh* mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];
        onEachMesh( buffer, mesh );
    }
}

void MeshLoader::onEachMesh( ObjectBufferList& buffer, const aiMesh* mesh ) {
    ObjectBuffer& bufferPart = buffer.buffers.emplace_back( ObjectBuffer{ } );

    bufferPart.vertexCount = mesh->mNumVertices * 3;

    Core::DynamicMemory memory{ mesh->mNumVertices * 5 * sizeof( float ) };

    for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
        const auto& vec = mesh->mVertices[ i ];

        memory.attachElements< float >( { vec.x, vec.y, vec.z } );

        if ( mesh->mTextureCoords[ 0 ] ) {
            const aiVector3D& textureCoordinates = mesh->mTextureCoords[ 0 ][ i ];

            memory.attachElements< float >( { textureCoordinates.x , textureCoordinates.y } );
        }
    }

    for ( unsigned int f = 0; f < mesh->mNumFaces; f++ ) {
        const aiFace& face = mesh->mFaces[ f ];

        for ( unsigned int j = 0; j < face.mNumIndices; j++ ) {
            bufferPart.indices.emplace_back( face.mIndices[ j ] );
        }
    }

    bufferPart.indexCount = bufferPart.indices.size();

    copyVertexBuffer( bufferPart, memory );
    if ( !bufferPart.indices.empty() ) {
        copyIndexBuffer( bufferPart, bufferPart.indices );
    }
}

void MeshLoader::copyVertexBuffer( ObjectBuffer &bufferPart, const Core::DynamicMemory &memory ) {
    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    RenderUtilities::initStagingBuffer( context, stagingBuffer, memory.data(), memory.size() );

    vk::BufferCreateInfo bufferCreateInfo{ };
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    bufferCreateInfo.size = memory.size( );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo{ };
    allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    bufferPart.vertexBuffer = context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->copyBuffer( memory.size(), stagingBuffer.first, bufferPart.vertexBuffer.first )
            ->execute( );

    context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
}

void MeshLoader::copyIndexBuffer( ObjectBuffer &bufferPart, const std::vector< uint32_t > &indices ) {
    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    RenderUtilities::initStagingBuffer( context, stagingBuffer, indices.data(), indices.size() * sizeof( uint32_t ) );

    vk::BufferCreateInfo bufferCreateInfo{ };
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    bufferCreateInfo.size = indices.size( ) * sizeof( uint32_t );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo{ };
    allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    bufferPart.indexBuffer = context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->copyBuffer( indices.size() * sizeof( uint32_t ), stagingBuffer.first, bufferPart.indexBuffer.first )
            ->execute( );

    context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
}

MeshLoader::~MeshLoader( ) {
    for ( auto& pair: loadedModels ) {
        for ( auto& buffer: pair.second.buffers ) {
            context->vma.destroyBuffer( buffer.vertexBuffer.first, buffer.vertexBuffer.second );
            if ( buffer.indexCount > 0 ) {
                context->vma.destroyBuffer( buffer.indexBuffer.first, buffer.indexBuffer.second );
            }
        }
    }
}


END_NAMESPACES