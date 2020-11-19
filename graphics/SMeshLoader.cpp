#include "SMeshLoader.h"
#include "../core/DynamicMemory.h"

NAMESPACES( SomeVulkan, Graphics )

void SMeshLoader::beforeFrame(  ObjectBuffer& objectBuffer, const ECS::CMesh& mesh ) {
    if ( loadedModels.find( mesh.path ) == loadedModels.end() ) {
        loadModel( mesh );
    }

    objectBuffer.parts.emplace( loadedModels[ mesh.path ].parts.begin() );
}

void SMeshLoader::loadModel( const SomeVulkan::ECS::CMesh& mesh ) {
    const aiScene* scene = importer.ReadFile( mesh.path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
        std::stringstream ss;
        ss << "ERROR::ASSIMP::" << importer.GetErrorString( );
        throw std::runtime_error( ss.str( ) );
    }

    loadedModels[ mesh.path ] = ObjectBuffer{ };
    onEachNode( loadedModels[ mesh.path ], scene, scene->mRootNode );
}


void SMeshLoader::onEachNode( ObjectBuffer& buffer, const aiScene* scene, const aiNode* pNode ) {
    for ( unsigned int i = 0; i < pNode->mNumChildren; ++i ) {
        onEachNode( buffer, scene, pNode->mChildren[ i ] );
    }

    for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ ) {
        const aiMesh* mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];
        onEachMesh( buffer, mesh );
    }
}

void SMeshLoader::onEachMesh( ObjectBuffer& buffer, const aiMesh* mesh ) {
    ObjectBufferPart& bufferPart = buffer.parts.emplace_back( ObjectBufferPart{ } );

    bufferPart.vertexCount = mesh->mNumVertices * 5;

    Core::DynamicMemory memory{ mesh->mNumVertices * 5 * sizeof( float ) };

    for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
        const auto& vec = mesh->mVertices[ i ];

        memory.attachElements< float >( { vec.x, vec.y, vec.z } );

        if ( mesh->mTextureCoords[ 0 ] ) {
            const aiVector3D& textureCoordinates = mesh->mTextureCoords[ 0 ][ i ];

            memory.attachElements< float >( { textureCoordinates.x , textureCoordinates.y } );
        }
    }

    std::vector< uint64_t > indices;

    for ( unsigned int f = 0; f < mesh->mNumFaces; f++ ) {
        const aiFace& face = mesh->mFaces[ f ];

        for ( unsigned int j = 0; j < face.mNumIndices; j++ ) {
            indices.emplace_back( face.mIndices[ j ] );
        }
    }

    bufferPart.indexCount = indices.size();

    copyVertexBuffer( bufferPart, memory );
    if ( !indices.empty() ) {
        copyIndexBuffer( bufferPart, indices );
    }
}

void SMeshLoader::copyVertexBuffer( ObjectBufferPart &bufferPart, const Core::DynamicMemory &memory ) {
    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    initStagingBuffer( stagingBuffer, memory.data(), memory.size() );

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
}

void SMeshLoader::copyIndexBuffer( ObjectBufferPart &bufferPart, const std::vector< uint64_t > &indices ) {
    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    initStagingBuffer( stagingBuffer, indices.data(), indices.size() );

    vk::BufferCreateInfo bufferCreateInfo{ };
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    bufferCreateInfo.size = indices.size( );
    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocationInfo{ };
    allocationInfo.usage = vma::MemoryUsage::eGpuOnly;
    allocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    bufferPart.vertexBuffer = context->vma.createBuffer( bufferCreateInfo, allocationInfo );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->copyBuffer( indices.size(), stagingBuffer.first, bufferPart.indexBuffer.first )
            ->execute( );
}

void SMeshLoader::initStagingBuffer( std::pair< vk::Buffer, vma::Allocation > &stagingBuffer, const void * data, const uint64_t &size ) {
    vk::BufferCreateInfo stagingBufferCreateInfo{ };

    stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stagingBufferCreateInfo.size = size;
    stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo stagingAllocationInfo{ };
    stagingAllocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    stagingAllocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
    stagingAllocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

    stagingBuffer = context->vma.createBuffer( stagingBufferCreateInfo, stagingAllocationInfo );

    void * deviceMemory = context->vma.mapMemory( stagingBuffer.second );
    memcpy( deviceMemory, data, size );
    context->vma.unmapMemory( stagingBuffer.second );
}

END_NAMESPACES