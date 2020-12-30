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

    frameResources.resize( renderDevice->getFrameCount( ) );
}

void GlobalResourceTable::resetTable( const std::shared_ptr< ECS::ComponentTable > &componentTable, const uint32_t &frameIndex )
{
/*    for ( auto it = frameResources[ frameIndex ].begin( ); it != frameResources[ frameIndex ].end( ); )
    {
        ShaderResourceWrapper &resourceWrapper = it->second;
        if ( resourceWrapper.isAllocated && resourceWrapper.ref->loadStrategy == ResourceLoadStrategy::LoadPerFrame )
        {
            resourceWrapper.ref->deallocate( );
            free( resourceWrapper.ref->dataAttachment->content );

            it = frameResources[ frameIndex ].erase( it );
        }
        else
        {
            ++it;
        }
    }*/

    currentComponentTable = componentTable;
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

    auto &geometryData = geometryList[ geometryIndex->second ];

    cleanGeometryData( geometryData );
}

void GlobalResourceTable::createGeometry( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    auto transformComponent = entity->getComponent< ECS::CTransform >( );
    auto meshComponent = entity->getComponent< ECS::CMesh >( );
    auto materialComponent = entity->getComponent< ECS::CMaterial >( );

    FUNCTION_BREAK( meshComponent == nullptr )

    MeshGeometry geometry = assetManager->getMeshGeometry( meshComponent->path );

    GeometryData &data = geometryList.emplace_back( GeometryData { } );

    /*
     * Create vertex data:
     */
    data.vertices = createResource( ResourceType::VertexData, ResourceLoadStrategy::LoadOnce );
    data.vertices->identifier = { "VertexData" };

    std::shared_ptr< VertexData > vertexData = std::make_shared< VertexData >( );
    vertexData->vertexCount = geometry.vertexCount;
    vertexData->content = geometry.vertices.data( );
    vertexData->size = geometry.vertices.size( ) * sizeof( float );

    data.vertices->dataAttachment = std::move( vertexData );
    data.vertices->bindStrategy = ResourceBindStrategy::BindPerObject;
    data.vertices->allocate( );
    // ---

    /*
     * Create index data:
     */
    if ( geometry.hasIndices )
    {
        data.indices = createResource( ResourceType::IndexData, ResourceLoadStrategy::LoadOnce );
        data.indices->identifier = { "IndexData" };
        data.indices->bindStrategy = ResourceBindStrategy::BindPerObject;
        std::shared_ptr< IndexData > indexData = std::make_shared< IndexData >( );
        indexData->indexCount = geometry.indices.size( );
        indexData->content = geometry.indices.data( );
        indexData->size = geometry.indices.size( ) * sizeof( uint32_t );
        data.indices->dataAttachment = std::move( indexData );
        data.indices->allocate( );
    }
    //---
    /*
     * Create material data:
     */

    if ( materialComponent != nullptr )
    {
        ShaderMaterialStruct shaderData { };
        shaderData.diffuseColor = materialComponent->diffuse;
        shaderData.specularColor = materialComponent->specular;
        shaderData.shininess = materialComponent->shininess;
        shaderData.textureScale = glm::vec4( transformComponent->scale, 1.0f );

        data.material = createResource( );
        data.material->identifier = { StaticVars::getInputName( StaticVars::Input::Material ) };
        data.material->bindStrategy = ResourceBindStrategy::BindPerObject;
        attachStructDataAttachment( data.material, DataAttachmentFormatter::formatMaterialComponent( materialComponent, transformComponent ), true );
        data.material->allocate( );
        //---
        /*
         * Create texture data:
         */
        data.textures.resize( materialComponent->textures.size( ) );

        for ( int i = 0; i < data.textures.size( ); ++i )
        {
            data.textures[ i ] = createResource( ResourceType::Sampler2D, ResourceLoadStrategy::LoadOnce, ResourcePersistStrategy::StoreOnDeviceMemory, ResourceShaderStage::Fragment );
            data.textures[ i ]->identifier = { "Texture", i + 1 };
            auto textureData = assetManager->getImage( materialComponent->textures[ i ].path );
            textureData->textureInfo = materialComponent->textures[ i ];
            data.textures[ i ]->dataAttachment = std::move( textureData );
            data.textures[ i ]->bindStrategy = ResourceBindStrategy::BindPerObject;
            data.textures[ i ]->allocate( );
        }
        //---
    }
    data.modelTransformPtr = transformComponent;

    entityGeometryMap[ entity->getUID( ) ] = geometryList.size( ) - 1;
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
    auto modelMat = DataAttachmentFormatter::formatModelMatrix( data.modelTransformPtr );
    memcpy( globalModelResourcePlaceholder->dataAttachment->content, &modelMat, globalModelResourcePlaceholder->dataAttachment->size );
}

void GlobalResourceTable::createEmptyImageResource( const OutputImage &image, const uint32_t &frameIndex )
{
    frameResources[ frameIndex ][ image.outputResourceName ] = { };
    auto &wrapper = frameResources[ frameIndex ][ image.outputResourceName ];

    wrapper.ref = createResource( );
    wrapper.isAllocated = true;
    wrapper.ref->type = ResourceType::Sampler2D;
    wrapper.ref->identifier = { image.outputResourceName };

    std::shared_ptr< SamplerDataAttachment > attachment = std::make_shared< SamplerDataAttachment >( );
    attachment->content = nullptr;
    attachment->width = image.width;
    attachment->height = image.height;

    wrapper.ref->dataAttachment = std::move( attachment );
    wrapper.ref->allocate( );
}

void GlobalResourceTable::allocateResource( const std::string &resourceName, const uint32_t &frameIndex )
{
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::GeometryData ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::Material ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::ModelMatrix ) )

    auto find = frameResources[ frameIndex ].find( resourceName );

    if ( find == frameResources[ frameIndex ].end( ) )
    {
        frameResources[ frameIndex ][ resourceName ] = { };
        find = frameResources[ frameIndex ].find( resourceName );
    }

    auto &wrapper = find->second;

    bool wrapperAllocatedThisFrame = !wrapper.isAllocated;
    if ( !wrapper.isAllocated )
    {
        wrapper.isAllocated = true;
        wrapper.ref = createResource( );
        wrapper.ref->identifier = { resourceName };
    }
    else
    {
        FUNCTION_BREAK( wrapper.ref->loadStrategy == ResourceLoadStrategy::LoadOnce )
    }

    if ( resourceName == StaticVars::getInputName( StaticVars::Input::EnvironmentLights ) )
    {
        attachStructDataAttachment( wrapper.ref, DataAttachmentFormatter::formatLightingEnvironment( currentComponentTable ), wrapperAllocatedThisFrame );
    }

    if ( resourceName == StaticVars::getInputName( StaticVars::Input::ViewProjection ) )
    {
        attachStructDataAttachment( wrapper.ref, DataAttachmentFormatter::formatCamera( currentComponentTable ), wrapperAllocatedThisFrame );
    }

    if ( resourceName == StaticVars::getInputName( StaticVars::Input::SkyBox ) )
    {
        wrapper.ref->type = ResourceType::CubeMap;

        std::shared_ptr< CubeMapDataAttachment > cubeMapAttachment = std::make_shared< CubeMapDataAttachment >( );

        const auto &cubeMaps = currentComponentTable->getComponents< ECS::CCubeMap >( );

        FUNCTION_BREAK( cubeMaps.empty( ) ) // Todo don't die here or the flow breaks

        for ( auto &cm: cubeMaps[ 0 ]->texturePaths )
        {
            cubeMapAttachment->images.push_back( assetManager->getImage( cm.path ) );
        }

        wrapper.ref->dataAttachment = std::move( cubeMapAttachment );
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

std::shared_ptr< ShaderResource > GlobalResourceTable::getResource( const std::string &resourceName, const uint32_t &frameIndex )
{
    if ( resourceName == StaticVars::getInputName( StaticVars::Input::ModelMatrix ) )
    {
        return globalModelResourcePlaceholder;
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
    geometryData.vertices->deallocate( );

    if ( geometryData.indices != nullptr )
    {
        geometryData.indices->deallocate( );
    }

    if ( geometryData.material != nullptr )
    {
        geometryData.material->deallocate( );
        free( geometryData.material->dataAttachment->content );
    }

    for ( auto &tex: geometryData.textures )
    {
        tex->deallocate( );
    }
}

std::vector< GeometryData > GlobalResourceTable::getGeometryList( )
{
    return geometryList;
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
                free( resourceWrapper.ref->dataAttachment->content );
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

    free( globalModelResourcePlaceholder->dataAttachment->content );
}

END_NAMESPACES