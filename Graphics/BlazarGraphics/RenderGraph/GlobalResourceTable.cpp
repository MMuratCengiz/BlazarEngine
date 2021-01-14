#include "GlobalResourceTable.h"

#include <utility>

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

    frameResources.resize( renderDevice->getFrameCount( ) );
    resourcesUpdatedThisFrame.resize( renderDevice->getFrameCount( ) );
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

    auto &geometryData = geometryList[ geometryIndex->second ];

    cleanGeometryData( geometryData );
}

void GlobalResourceTable::createGeometry( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    FUNCTION_BREAK( !entity->hasComponent< ECS::CMesh >( ) )
    FUNCTION_BREAK( entity->hasComponent< ECS::CCubeMap >( ) )

    geometryList.emplace_back( std::move( createGeometryData( entity ) ) );

    entityGeometryMap[ entity->getUID( ) ] = geometryList.size( ) - 1;
}

GeometryData GlobalResourceTable::createGeometryData( const std::shared_ptr< ECS::IGameEntity > &entity, const std::string& outputGeometry )
{
    auto transformComponent = entity->getComponent< ECS::CTransform >( );
    auto meshComponent = entity->getComponent< ECS::CMesh >( );
    auto materialComponent = entity->getComponent< ECS::CMaterial >( );

    MeshGeometry geometry = assetManager->getMeshGeometry( meshComponent->path );

    std::string parentBoundingName = StaticVars::getInputName( StaticVars::Input::GeometryData );

    if ( outputGeometry == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube ) )
    {
        parentBoundingName = "ScreenCube";
    }

    if ( outputGeometry == BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare ) )
    {
        parentBoundingName = "ScreenQuad";
    }

    GeometryData data { };
    /*
     * Create vertex data:
     */
    auto vertices = createResource( ResourceType::VertexData, ResourceLoadStrategy::LoadOnce );

    vertices->identifier = { "VertexData" };

    std::shared_ptr< VertexData > vertexData = std::make_shared< VertexData >( );
    vertexData->vertexCount = geometry.vertexCount;
    vertexData->content = geometry.vertices.data( );
    vertexData->size = geometry.vertices.size( ) * sizeof( float );

    vertices->dataAttachment = std::move( vertexData );
    vertices->bindStrategy = ResourceBindStrategy::BindPerObject;
    vertices->allocate( );

    data.resources.push_back( { true, parentBoundingName, std::move( vertices ) } );
    // ---

    /*
     * Create index data:
     */
    if ( geometry.hasIndices )
    {
        auto indices = createResource( ResourceType::IndexData, ResourceLoadStrategy::LoadOnce );
        indices->identifier = { "IndexData" };
        indices->bindStrategy = ResourceBindStrategy::BindPerObject;
        std::shared_ptr< IndexData > indexData = std::make_shared< IndexData >( );
        indexData->indexCount = geometry.indices.size( );
        indexData->content = geometry.indices.data( );
        indexData->size = geometry.indices.size( ) * sizeof( uint32_t );
        indices->dataAttachment = std::move( indexData );
        indices->allocate( );

        data.resources.push_back( { true, parentBoundingName, std::move( indices ) } );
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

        auto material = createResource( );
        material->identifier = { StaticVars::getInputName( StaticVars::Input::Material ) };
        material->bindStrategy = ResourceBindStrategy::BindPerObject;
        attachStructDataAttachment( material, DataAttachmentFormatter::formatMaterialComponent( materialComponent, transformComponent ), true );
        material->allocate( );

        data.resources.push_back( { true, "Material", std::move( material ) } );
        //---
        /*
         * Create texture data:
         */
        for ( int i = 0; i < materialComponent->textures.size( ); ++i )
        {
            auto texture = createResource( ResourceType::Sampler2D, ResourceLoadStrategy::LoadOnce, ResourcePersistStrategy::StoreOnDeviceMemory, ResourceShaderStage::Fragment );
            texture->identifier = { "Texture", i + 1 };
            auto textureData = assetManager->getImage( materialComponent->textures[ i ].path );
            textureData->textureInfo = materialComponent->textures[ i ];
            texture->dataAttachment = std::move( textureData );
            texture->bindStrategy = ResourceBindStrategy::BindPerObject;
            texture->allocate( );

            data.resources.push_back( { true, "Material", std::move( texture ) } );
        }
        //---
        /*
         * Create HeightMap data:
         */
        if ( !materialComponent->heightMap.path.empty( ) )
        {
            auto texture = createResource( ResourceType::Sampler2D, ResourceLoadStrategy::LoadOnce, ResourcePersistStrategy::StoreOnDeviceMemory, ResourceShaderStage::Vertex );
            texture->identifier = { "HeightMap" };
            auto textureData = assetManager->getImage( materialComponent->heightMap.path );
            textureData->textureInfo = materialComponent->heightMap;
            texture->dataAttachment = std::move( textureData );
            texture->bindStrategy = ResourceBindStrategy::BindPerObject;
            texture->allocate( );

            data.resources.push_back(  { true, "Material", std::move( texture ) } );
        }
        //---
    }
    data.modelTransformPtr = transformComponent;
    data.referenceEntity = entity;

    return std::move( data );
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

    auto normalMat = DataAttachmentFormatter::formatNormalMatrix( data.modelTransformPtr );
    memcpy( globalNormalModelResourcePlaceholder->dataAttachment->content, &normalMat, globalModelResourcePlaceholder->dataAttachment->size );
}

void GlobalResourceTable::createEmptyImageResource( const OutputImage &image, const uint32_t &frameIndex )
{
    frameResources[ frameIndex ][ image.outputResourceName ] = { };
    auto &wrapper = frameResources[ frameIndex ][ image.outputResourceName ];

    wrapper.ref = createResource( ResourceType::Sampler2D );
    wrapper.isAllocated = true;
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
    // Todo maybe specify which ones are geometry data that is specified else where
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::GeometryData ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::Material ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::ModelMatrix ) )
    FUNCTION_BREAK( resourceName == StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) )
    FUNCTION_BREAK( resourceName == "Texture1" )
    FUNCTION_BREAK( resourceName == "HeightMap" )

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

    bool isEnvironmentLights = resourceName == StaticVars::getInputName( StaticVars::Input::EnvironmentLights );
    bool isViewProjection = resourceName == StaticVars::getInputName( StaticVars::Input::ViewProjection );
    bool isSkyBox = resourceName == StaticVars::getInputName( StaticVars::Input::SkyBox );

    if ( !wrapper.isAllocated )
    {

        ResourceType resourceType = ResourceType::Uniform;

        if ( isSkyBox )
        {
            resourceType = ResourceType::CubeMap;
        }

        wrapper.isAllocated = true;
        wrapper.ref = createResource( resourceType );
        wrapper.ref->identifier = { resourceName };
    }
    else
    {
        FUNCTION_BREAK( wrapper.ref->loadStrategy == ResourceLoadStrategy::LoadOnce )
    }

    auto customFormatter = customFormatters.find( resourceName );
    if ( customFormatter != customFormatters.end( ) )
    {
        if ( wrapperAllocatedThisFrame )
        {
            wrapper.ref->dataAttachment = std::make_shared< IDataAttachment >( );
        }

        std::shared_ptr< IDataAttachment > &dataAttachment = wrapper.ref->dataAttachment;

        free( dataAttachment->content );

        auto content = customFormatter->second( currentComponentTable );

        dataAttachment->size = content.size;
        dataAttachment->content = content.data;

        wrapper.ref->type = ResourceType::Uniform; // Todo maybe dynamic
    }

    if ( isEnvironmentLights )
    {
        attachStructDataAttachment( wrapper.ref, DataAttachmentFormatter::formatLightingEnvironment( currentComponentTable ), wrapperAllocatedThisFrame );
    }

    if ( isViewProjection )
    {
        attachStructDataAttachment( wrapper.ref, DataAttachmentFormatter::formatCamera( currentComponentTable ), wrapperAllocatedThisFrame );
    }

    if ( isSkyBox )
    {
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

    if ( resourceName == StaticVars::getInputName( StaticVars::Input::NormalModelMatrix ) )
    {
        return globalNormalModelResourcePlaceholder;
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
    for ( auto & resource: geometryData.resources )
    {
        resource.ref->deallocate( );

        /* todo wtf crashes
        if ( resource.ref->dataAttachment != nullptr && resource.ref->dataAttachment->content && !resource.ref->dataAttachment->autoFree)
        {
            free( resource.ref->dataAttachment->content );
        }*/
    }
}

void GlobalResourceTable::registerCustomFormatter( const std::string &resourceName, FormatterFunc func )
{
    customFormatters[ resourceName ] = std::move( func );
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
        auto ptr = std::make_shared< ECS::DynamicGameEntity >( );
        ptr->createComponent< ECS::CMesh >( );
        ptr->getComponent< ECS::CMesh >( )->path = outputGeometry;

        const auto &assetGeometry = assetManager->getMeshGeometry( outputGeometry );

        outputGeometryMap[ outputGeometry ] = createGeometryData( std::move( ptr ), outputGeometry );

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

    for ( auto &geometry: outputGeometryMap )
    {
        cleanGeometryData( geometry.second );
    }

    free( globalModelResourcePlaceholder->dataAttachment->content );
    free( globalNormalModelResourcePlaceholder->dataAttachment->content );
}

END_NAMESPACES