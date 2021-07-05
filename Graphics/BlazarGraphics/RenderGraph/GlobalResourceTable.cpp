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

#include "GlobalResourceTable.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

/*
 * TODO clean up all these mallocs
 */

GlobalResourceTable::GlobalResourceTable( IRenderDevice *renderDevice, AssetManager *assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
    globalModelResourcePlaceholder = createResource( );
    globalModelResourcePlaceholder->type = ResourceType::PushConstant;
    globalModelResourcePlaceholder->identifier = { StaticVars::getInputName( StaticVars::Input::ModelMatrix ) };
    globalModelResourcePlaceholder->dataAttachment = std::make_shared< IDataAttachment >( );
    globalModelResourcePlaceholder->dataAttachment->size = 4 * 4 * sizeof( float );
    globalModelResourcePlaceholder->dataAttachment->content = malloc( 4 * 4 * sizeof( float ) );
    globalModelResourcePlaceholder->bindStrategy = ResourceBindStrategy::BindPerObject;

    globalNormalModelResourcePlaceholder = createResource( );
    globalNormalModelResourcePlaceholder->type = ResourceType::PushConstant;
    globalNormalModelResourcePlaceholder->identifier = { StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) };
    globalNormalModelResourcePlaceholder->dataAttachment = std::make_shared< IDataAttachment >( );
    globalNormalModelResourcePlaceholder->dataAttachment->size = 4 * 4 * sizeof( float );
    globalNormalModelResourcePlaceholder->dataAttachment->content = malloc( 4 * 4 * sizeof( float ) );
    globalNormalModelResourcePlaceholder->bindStrategy = ResourceBindStrategy::BindPerObject;

    globalBoneTransformationsResourcePlaceholder = createResource( );
    globalBoneTransformationsResourcePlaceholder->type = ResourceType::Uniform;
    globalBoneTransformationsResourcePlaceholder->identifier = { "BoneTransformations" };
    globalBoneTransformationsResourcePlaceholder->dataAttachment = std::make_shared< IDataAttachment >( );
    globalBoneTransformationsResourcePlaceholder->dataAttachment->size = sizeof( BoneTransformations );
    globalBoneTransformationsResourcePlaceholder->dataAttachment->content = malloc( sizeof( BoneTransformations ) );
    globalBoneTransformationsResourcePlaceholder->bindStrategy = ResourceBindStrategy::BindPerObject;
    auto boneTransformations = BoneTransformations { };
    memcpy( globalBoneTransformationsResourcePlaceholder->dataAttachment->content, &boneTransformations, globalBoneTransformationsResourcePlaceholder->dataAttachment->size );
    globalBoneTransformationsResourcePlaceholder->allocate( );

    frameResources.resize( renderDevice->getFrameCount( ) );
    resourcesUpdatedThisFrame.resize( renderDevice->getFrameCount( ) );

    resourceBinder = std::make_unique< ResourceBinder >( );
}

void GlobalResourceTable::resetTable( const std::shared_ptr< ECS::ComponentTable > &componentTable, const uint32_t &frameIndex )
{
    currentComponentTable = componentTable;
    resourcesUpdatedThisFrame.clear( );
    resourcesUpdatedThisFrame.resize( renderDevice->getFrameCount( ), { } );
}

void GlobalResourceTable::addEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    createGeometry( entity );
    createGeometryList( entity->getChildren( ) );
}

void GlobalResourceTable::updateEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    removeEntity( entity );
    addEntity( entity );
}

void GlobalResourceTable::removeEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    auto geometryIndex = entityGeometryMap.find( entity->getUID( ) );

    FUNCTION_BREAK( geometryIndex == entityGeometryMap.end( ) );

    for ( int idx: geometryIndex->second )
    {
        auto &geometryData = geometryList[ idx ];

        cleanGeometryData( geometryData );
    }
}

void GlobalResourceTable::createGeometry( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    FUNCTION_BREAK( !entity->hasComponent< ECS::CMesh >( ) )
    FUNCTION_BREAK( entity->hasComponent< ECS::CCubeMap >( ) )

    const std::vector< GeometryData > &entityGeometries = createGeometryData( entity );

    geometryList.insert( geometryList.end(), entityGeometries.begin(), entityGeometries.end() );

    entityGeometryMap[ entity->getUID( ) ] = { };

    for ( uint32_t i = geometryList.size() - entityGeometries.size( ); i < geometryList.size( ); ++i )
    {
        entityGeometryMap[ entity->getUID( ) ].push_back( i );
    }
}

std::vector< GeometryData > GlobalResourceTable::createGeometryData( const std::shared_ptr< ECS::IGameEntity > &entity, const std::string &outputGeometry )
{
    auto transformComponent = entity->getComponent< ECS::CTransform >( );
    auto meshComponent = entity->getComponent< ECS::CMesh >( );
    auto materialComponent = entity->getComponent< ECS::CMaterial >( );
    auto tessellationComponent = entity->getComponent< ECS::CTessellation >( );

    std::string parentBoundingName = StaticVars::getInputName( StaticVars::Input::GeometryData );

    if ( outputGeometry == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube ) )
    {
        parentBoundingName = "ScreenCube";
    }

    if ( outputGeometry == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare ) )
    {
        parentBoundingName = "ScreenQuad";
    }

    if ( outputGeometry == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle ) )
    {
        parentBoundingName = "ScreenOversizedTriangle";
    }

    MeshGeometry geometry = assetManager->getMeshGeometry( meshComponent->geometryRefIdx, meshComponent->path );

    std::vector< GeometryData > result;

    for ( SubMeshGeometry & subMeshGeometry : geometry.subGeometries )
    {
        result.push_back( std::move( createGeometryData( entity, transformComponent, parentBoundingName, subMeshGeometry ) )) ;
    }

    return std::move( result );
}

GeometryData GlobalResourceTable::createGeometryData( const std::shared_ptr< ECS::IGameEntity > &entity, const std::shared_ptr< ECS::CTransform > &transformComponent, const std::string &parentBoundingName,
                                                      SubMeshGeometry &subMeshGeometry )
{
    GeometryData data { };

    /*
    * Create vertex data:
    */
    auto vertices = createResource( ResourceType::VertexData, ResourceLoadStrategy::LoadOnce );

    vertices->identifier = { "VertexData" };

    std::shared_ptr< VertexData > vertexData = std::make_shared< VertexData >( );
    vertexData->vertexCount = subMeshGeometry.vertexCount;
    vertexData->content = subMeshGeometry.dataRaw.data( );
    vertexData->size = subMeshGeometry.dataRaw.size( ) * sizeof( float );

    vertices->dataAttachment = std::move( vertexData );
    vertices->bindStrategy = ResourceBindStrategy::BindPerObject;
    vertices->allocate( );

    data.resources.push_back( { true, parentBoundingName, std::move( vertices ) } );
    // ---

    /*
     * Create index data:
     */
    if ( !subMeshGeometry.indices.empty() )
    {
        auto indices = createResource( ResourceType::IndexData, ResourceLoadStrategy::LoadOnce );
        indices->identifier = { "IndexData" };
        indices->bindStrategy = ResourceBindStrategy::BindPerObject;
        std::shared_ptr< IndexData > indexData = std::make_shared< IndexData >( );
        indexData->indexCount = subMeshGeometry.indices.size( );
        indexData->content = subMeshGeometry.indices.data( );
        indexData->size = subMeshGeometry.indices.size( ) * sizeof( uint32_t );
        indices->dataAttachment = std::move( indexData );
        indices->allocate( );

        data.resources.push_back( { true, parentBoundingName, std::move( indices ) } );
    }

    for ( const std::string &resourceName: resourceBinder->getAllPerEntityBinders( ) )
    {
        // Todo load once is not respected here
        auto resource = createResource( resourceBinder->getResourceType( resourceName ) );

        resource->identifier = { resourceName };
        resource->bindStrategy = ResourceBindStrategy::BindPerObject;

        bool validResource = true;

        if ( resource->type == ResourceType::Uniform )
        {
            auto binder = resourceBinder->getResourcePerEntityUniformBinder( resourceName );

            auto content = binder( entity );

            validResource = content.size != 0;

            if ( validResource )
            {
                resource->dataAttachment = std::make_shared< IDataAttachment >( );
                resource->dataAttachment->size = content.size;
                resource->dataAttachment->content = content.data;
            }
        }
        else if ( resource->type == ResourceType::Sampler2D )
        {
            auto binder = resourceBinder->getResourcePerEntityTextureBinder( resourceName );

            auto content = binder( entity );

            validResource = !content.textures.empty( );

            if ( validResource )
            {
                auto textureData = getSamplerDataAttachment( content.textures[ 0 ] );

                textureData->textureInfo = content.textures[ 0 ];
                resource->dataAttachment = std::move( textureData );
            }
        }

        if ( validResource )
        {
            resource->allocate( );
            data.resources.push_back( { true, resourceName, std::move( resource ) } );
        }
    }

    data.modelTransformPtr = transformComponent;
    data.referenceEntity = entity;

    return data;
}

void GlobalResourceTable::createGeometryList( const std::vector< std::shared_ptr< ECS::IGameEntity > > &entities )
{
    for ( const auto &entity: entities )
    {
        createGeometry( entity );
        createGeometryList( entity->getChildren( ) );
    }
}

std::shared_ptr< ShaderResource > GlobalResourceTable::createResource( const ResourceType &type,
                                                                       const ResourceLoadStrategy &loadStrategy,
                                                                       const ResourcePersistStrategy &persistStrategy,
                                                                       const ResourceShaderStage &shaderStage )
{
    ShaderResourceRequest shaderResourceRequest { };
    shaderResourceRequest.type = type;
    shaderResourceRequest.loadStrategy = loadStrategy;
    shaderResourceRequest.persistStrategy = persistStrategy;
    shaderResourceRequest.shaderStage = shaderStage;

    return renderDevice->getResourceProvider( )->createResource( shaderResourceRequest );
}

void GlobalResourceTable::setActiveGeometryModel( const GeometryData &data )
{
    auto modelMat = DataAttachmentFormatter::formatModelMatrix( data.modelTransformPtr, data.referenceEntity );
    memcpy( globalModelResourcePlaceholder->dataAttachment->content, &modelMat, globalModelResourcePlaceholder->dataAttachment->size );

    auto normalMat = DataAttachmentFormatter::formatNormalMatrix( data.modelTransformPtr, data.referenceEntity );
    memcpy( globalNormalModelResourcePlaceholder->dataAttachment->content, &normalMat, globalModelResourcePlaceholder->dataAttachment->size );

    auto boneTransformations = DataAttachmentFormatter::formatBoneTransformations( data.referenceEntity );
    memcpy( globalBoneTransformationsResourcePlaceholder->dataAttachment->content, &boneTransformations, globalBoneTransformationsResourcePlaceholder->dataAttachment->size );
    globalBoneTransformationsResourcePlaceholder->update( );
}

void GlobalResourceTable::allocateResource( const std::string &resourceName, const uint32_t &frameIndex )
{
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::GeometryData ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::ModelMatrix ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) )
    FUNCTION_BREAK( resourceName == "BoneTransformations" )

    auto bindType = resourceBinder->getResourceBindType( resourceName );
    FUNCTION_BREAK( bindType == ResourceBindType::PerEntityTexture || bindType == ResourceBindType::PerEntityUniform  )

    auto resourceUpdatedThisFrame = resourcesUpdatedThisFrame[ frameIndex ].find( resourceName );

    FUNCTION_BREAK ( resourceUpdatedThisFrame != resourcesUpdatedThisFrame[ frameIndex ].end( ) )

    resourcesUpdatedThisFrame[ frameIndex ][ resourceName ] = true;

    auto find = frameResources[ frameIndex ].find( resourceName );

    if ( find == frameResources[ frameIndex ].end( ) )
    {
        frameResources[ frameIndex ][ resourceName ] = { };
        find = frameResources[ frameIndex ].find( resourceName );
    }

    auto &wrapper = find->second;

    bool wrapperAllocatedThisFrame = !wrapper.isAllocated;

    ResourceType resourceType = resourceBinder->getResourceType( resourceName );

    if ( wrapperAllocatedThisFrame )
    {
        wrapper.isAllocated = true;
        wrapper.ref = createResource( resourceType );
        wrapper.ref->identifier = { resourceName };
        wrapper.ref->type = resourceType;
    }
    else
    {
        FUNCTION_BREAK( wrapper.ref->loadStrategy == ResourceLoadStrategy::LoadOnce )
    }

    if ( wrapperAllocatedThisFrame )
    {
        if ( wrapper.ref->type == ResourceType::Uniform )
        {
            wrapper.ref->dataAttachment = std::make_shared< IDataAttachment >( );
        }
        if ( wrapper.ref->type == ResourceType::CubeMap )
        {
            wrapper.ref->dataAttachment = std::make_shared< CubeMapDataAttachment >( );
        }
        if ( wrapper.ref->type == ResourceType::Sampler2D )
        {
            wrapper.ref->dataAttachment = std::make_shared< SamplerDataAttachment >( );
        }
    }

    if ( wrapper.ref->type == ResourceType::Uniform )
    {
        wrapper.ref->dataAttachment = std::make_shared< IDataAttachment >( );
        std::shared_ptr< IDataAttachment > &dataAttachment = wrapper.ref->dataAttachment;

        free( dataAttachment->content );

        // Per entity
        auto binder = resourceBinder->getResourcePerFrameUniformBinder( resourceName );
        auto content = binder( currentComponentTable );

        dataAttachment->size = content.size;
        dataAttachment->content = content.data;
    }

    if ( wrapper.ref->type == ResourceType::CubeMap )
    {
        std::shared_ptr< CubeMapDataAttachment > cubeMapAttachment = std::make_shared< CubeMapDataAttachment >( );

        auto binder = resourceBinder->getResourcePerFrameTextureBinder( resourceName );
        auto content = binder( currentComponentTable );

        for ( const auto &texture: content.textures )
        {
            cubeMapAttachment->images.push_back( assetManager->getImage( texture.path ) );
        }

        wrapper.ref->dataAttachment = std::move( cubeMapAttachment );
    }

    if ( wrapper.ref->type == ResourceType::Sampler2D )
    {
        std::shared_ptr< SamplerDataAttachment > samplerAttachment;

        auto binder = resourceBinder->getResourcePerFrameTextureBinder( resourceName );
        auto content = binder( currentComponentTable );

        for ( const auto &texture: content.textures )
        {
            samplerAttachment = getSamplerDataAttachment( texture );
        }

        wrapper.ref->dataAttachment = std::move( samplerAttachment );
    }

    if ( wrapperAllocatedThisFrame )
    {
        wrapper.ref->allocate( );
    }
    else
    {
        wrapper.ref->update( );
    }
}

std::shared_ptr< SamplerDataAttachment > GlobalResourceTable::getSamplerDataAttachment( const ECS::Material::TextureInfo &texture )
{
    std::shared_ptr< SamplerDataAttachment > samplerAttachment;

    if ( texture.isInMemory )
    {
        samplerAttachment = std::make_shared< SamplerDataAttachment >( );
        samplerAttachment->content = texture.inMemoryTexture.contents;
        samplerAttachment->width = texture.inMemoryTexture.width;
        samplerAttachment->height = texture.inMemoryTexture.height;
        samplerAttachment->channels = texture.inMemoryTexture.channels;

        switch ( texture.inMemoryTexture.format )
        {
            case ECS::Material::ImageFormat::R8G8B8A8Unorm:
                samplerAttachment->format = ResourceImageFormat::R8G8B8A8Unorm;
                break;
            case ECS::Material::ImageFormat::R8G8B8Unorm:
                samplerAttachment->format = ResourceImageFormat::R8G8B8Unorm;
                break;
            case ECS::Material::ImageFormat::R8G8Unorm:
                samplerAttachment->format = ResourceImageFormat::R8G8Unorm;
                break;
            case ECS::Material::ImageFormat::R8Unorm:
                samplerAttachment->format = ResourceImageFormat::R8Unorm;
                break;
        }
    }
    else
    {
        samplerAttachment = assetManager->getImage( texture.path );
        samplerAttachment->textureInfo = texture;
    }

    return samplerAttachment;
}

std::shared_ptr< ShaderResource > GlobalResourceTable::getResource( const std::string &resourceName, const uint32_t &frameIndex )
{
    if ( resourceName == StaticVars::getInputName( StaticVars::Input::ModelMatrix ) )
    {
        return globalModelResourcePlaceholder;
    }

    if ( resourceName == StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) )
    {
        return globalNormalModelResourcePlaceholder;
    }

    if ( resourceName == "BoneTransformations" )
    {
        return globalBoneTransformationsResourcePlaceholder;
    }

    auto findIt = frameResources[ frameIndex ].find( resourceName );

    if ( findIt == frameResources[ frameIndex ].end( ) )
    {
        return nullptr;
    }

    if ( !findIt->second.isAllocated )
    {
        findIt->second.ref->allocate( );
    }

    return findIt->second.ref;
}

void GlobalResourceTable::prepareResource( const std::string &resourceName, const ResourceUsage &usage, const uint32_t &frameIndex )
{
    frameResources[ frameIndex ][ resourceName ].ref->prepareForUsage( usage );
}

void GlobalResourceTable::cleanGeometryData( GeometryData &geometryData )
{
    for ( auto &resource: geometryData.resources )
    {
        resource.ref->deallocate( );

        /* todo wtf crashes
        if ( resource.ref->dataAttachment != nullptr && resource.ref->dataAttachment->content && !resource.ref->dataAttachment->autoFree)
        {
            free( resource.ref->dataAttachment->content );
        }*/
    }
}

std::vector< GeometryData > GlobalResourceTable::getGeometryList( )
{
    return geometryList;
}

std::vector< GeometryData > GlobalResourceTable::getOutputGeometryList( const std::string &outputGeometry )
{
    std::vector< GeometryData > geometryData;

    auto find = outputGeometryMap.find( outputGeometry );

    if ( find == outputGeometryMap.end( ) )
    {
        // Todo kind of a hack, see if it can't be fixed
        auto ptr = std::make_shared< ECS::DynamicGameEntity >( );
        ptr->createComponent< ECS::CMesh >( );
        ptr->getComponent< ECS::CMesh >( )->path = outputGeometry;

        const auto &assetGeometry = assetManager->getMeshGeometry( ptr->getComponent< ECS::CMesh >( )->geometryRefIdx, ptr->getComponent< ECS::CMesh >( )->path );

        outputGeometryMap[ outputGeometry ] = createGeometryData( std::move( ptr ), outputGeometry )[ 0 ];

        geometryData.emplace_back( outputGeometryMap[ outputGeometry ] );
    }
    else
    {
        geometryData.emplace_back( find->second );
    }

    return geometryData;
}

GlobalResourceTable::~GlobalResourceTable( )
{
    for ( uint32_t frameIndex = 0; frameIndex < renderDevice->getFrameCount( ); ++frameIndex )
    {
        for ( auto it = frameResources[ frameIndex ].begin( ); it != frameResources[ frameIndex ].end( ); )
        {
            ShaderResourceWrapper resourceWrapper = it->second;
            if ( resourceWrapper.isAllocated && resourceWrapper.ref->loadStrategy == ResourceLoadStrategy::LoadPerFrame )
            {
                resourceWrapper.ref->deallocate( );
                if ( !resourceWrapper.ref->dataAttachment->autoFree && resourceWrapper.ref->dataAttachment->size != 0 )
                {
                    free( resourceWrapper.ref->dataAttachment->content );
                }
                it = frameResources[ frameIndex ].erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    for ( auto &geometry: geometryList )
    {
        cleanGeometryData( geometry );
    }

    for ( auto &geometry: outputGeometryMap )
    {
        cleanGeometryData( geometry.second );
    }

    globalBoneTransformationsResourcePlaceholder->deallocate( );
    free( globalModelResourcePlaceholder->dataAttachment->content );
    free( globalNormalModelResourcePlaceholder->dataAttachment->content );
    free( globalBoneTransformationsResourcePlaceholder->dataAttachment->content );
}

END_NAMESPACES