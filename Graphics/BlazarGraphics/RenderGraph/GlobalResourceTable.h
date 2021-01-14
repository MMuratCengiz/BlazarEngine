#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include "StaticVars.h"
#include "../IRenderDevice.h"
#include "../AssetManager.h"
#include "../DataAttachmentFormatter.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct ShaderResourceWrapper
{
    bool isAllocated = false;
    std::string boundResourceName;
    std::shared_ptr< ShaderResource > ref;
};

struct GeometryData
{
    std::vector< ShaderResourceWrapper > resources;
    std::shared_ptr< ECS::CTransform > modelTransformPtr;
    std::shared_ptr< ECS::IGameEntity > referenceEntity;
};

// Mappings to shader versions:
struct ShaderMaterialStruct
{
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    glm::vec4 textureScale;

    float shininess;
};

struct AttachmentContent
{
    char * data;
    uint32_t size;
    ResourceType resourceType;
};

typedef std::function< AttachmentContent( const std::shared_ptr< ECS::ComponentTable > &components  ) > FormatterFunc;

class GlobalResourceTable
{
private:
    AssetManager * assetManager;
    IRenderDevice * renderDevice;
    std::shared_ptr< ECS::ComponentTable > currentComponentTable;

    std::vector< std::unordered_map< std::string, ShaderResourceWrapper > > frameResources;
    std::vector< std::unordered_map< std::string, bool > > resourcesUpdatedThisFrame;

    std::vector< GeometryData > geometryList;
    std::unordered_map< uint64_t, uint32_t > entityGeometryMap;
    std::unordered_map< std::string, GeometryData > outputGeometryMap;
    std::unordered_map< std::string, FormatterFunc > customFormatters;

    std::shared_ptr< ShaderResource > globalModelResourcePlaceholder;
    std::shared_ptr< ShaderResource > globalNormalModelResourcePlaceholder;
public:
    explicit GlobalResourceTable( IRenderDevice* renderDevice, AssetManager* assetManager );

    void addEntity( const std::shared_ptr< ECS::IGameEntity > &entity );
    void updateEntity( const std::shared_ptr< ECS::IGameEntity > &entity );
    void removeEntity( const std::shared_ptr< ECS::IGameEntity > &entity );
    void registerCustomFormatter( const std::string& resourceName, FormatterFunc func );
    void prepareResource( const std::string &resourceName, const ResourceUsage &usage, const uint32_t& frameIndex );
    void allocateResource( const std::string& resourceName, const uint32_t& frameIndex );
    void createEmptyImageResource( const OutputImage& image, const uint32_t& frameIndex );
    void resetTable( const std::shared_ptr< ECS::ComponentTable >& componentTable, const uint32_t& frameIndex );
    void setActiveGeometryModel( const GeometryData &data );

    std::shared_ptr< ShaderResource > getResource( const std::string& resourceName, const uint32_t& frameIndex );
    std::vector< GeometryData > getGeometryList( );
    std::vector< GeometryData > getOutputGeometryList( const std::string &outputGeometry );

    ~GlobalResourceTable( );
private:
    std::shared_ptr< ShaderResource > createResource( const ResourceType& type = ResourceType::Uniform,
                                                      const ResourceLoadStrategy& loadStrategy = ResourceLoadStrategy::LoadPerFrame,
                                                      const ResourcePersistStrategy& persistStrategy = ResourcePersistStrategy::StoreOnDeviceMemory,
                                                      const ResourceShaderStage& shaderStage = ResourceShaderStage::Vertex );

    template< typename StructType >
    static inline void attachStructDataAttachment( const std::shared_ptr< ShaderResource > &resource, StructType structData, bool allocateMemory )
    {
        if ( allocateMemory )
        {
            resource->dataAttachment = std::make_shared< IDataAttachment >( );
        }

        std::shared_ptr< IDataAttachment > &dataAttachment = resource->dataAttachment;
        if ( allocateMemory )
        {
            dataAttachment->size = sizeof( StructType );
            dataAttachment->content = malloc( dataAttachment->size );
        }

        memcpy( dataAttachment->content, &structData, dataAttachment->size );
    }

    GeometryData createGeometryData( const std::shared_ptr< ECS::IGameEntity > &entity, const std::string& outputGeometry = "" );
    void createGeometry( const std::shared_ptr< ECS::IGameEntity > &entity );
    void createGeometryList( const std::vector< std::shared_ptr< ECS::IGameEntity > > &entities );
    static void cleanGeometryData( GeometryData &geometryData ) ;
};

END_NAMESPACES

