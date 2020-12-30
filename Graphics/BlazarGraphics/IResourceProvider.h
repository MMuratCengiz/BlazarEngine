#pragma once

#include <BlazarCore/Common.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

/*
 * Note ResourceType::GeometryData can only be bound to shader stage: ResourceShaderStage::Vertex
 */
enum class ResourceShaderStage
{
    Vertex,
    Fragment,
    Compute
};

enum class ResourceType
{
    VertexData,
    IndexData,
    CubeMap,
    PushConstant,
    Sampler2D,
    Sampler3D,
    Uniform
};

enum class ResourceLoadStrategy
{
    LoadPerFrame,
    LoadOnce,
    LoadOnUpdate
};

enum class ResourceBindStrategy
{
    BindPerFrame,
    BindPerObject
};

enum class ResourceLockType
{
    Fence,
    Semaphore
};

enum class ResourceUsage
{
    None,
    RenderTarget,
    ShaderInputSampler2D
};

enum class ResourceImageFormat
{
    MatchSwapChainImageFormat,
    R16G16B16A16Sfloat,
    R8G8B8A8Unorm
};

class IResourceLock
{
protected:
    const ResourceLockType lockType;
public:
    explicit inline IResourceLock( const ResourceLockType &lockType ) : lockType( lockType )
    { }

    virtual void wait( ) = 0;
    virtual void reset( ) = 0;
    virtual void notify( ) = 0;

    virtual ~IResourceLock( ) = default;;
};

/*
 * StoreOnHostMemory means this resource will be kept on System RAM until manually deallocated
 * StoreOnDeviceMemory means this resource will be kept on Device Memory until manually deallocated
 * TransitOnHostMemory means this resource will be continuously passed to System RAM that is also Device Visible. Should be used for small data
 * TransitOnDeviceMemory means this resource will be continuously passed to Device Memory each frame on a generic buffer, this should not be used but exists as a place holder
 */
enum class ResourcePersistStrategy
{
    StoreOnHostMemory,
    StoreOnDeviceMemory,
    TransitOnHostMemory,
    TransitOnDeviceMemory,
};

struct ResourceIdentifier
{
    std::string name;
    int deviation = -1;

    [[nodiscard]] inline std::string getKey( ) const
    {
        if ( deviation == -1 )
        {
            return name;
        }

        std::stringstream str;

        str << name << deviation;

        return str.str( );
    }
};

struct ShaderResourceRequest
{
    ResourceType type;
    ResourceLoadStrategy loadStrategy;
    ResourcePersistStrategy persistStrategy;
    ResourceShaderStage shaderStage;
};

struct IDataAttachment
{
    bool autoFree = true;

    void *content { };
    uint64_t size { };

    virtual ~IDataAttachment( )
    {
        if ( autoFree )
        {
            free( content );
            content = nullptr;
        }
    }

    explicit inline IDataAttachment( bool autoFree = true ) : autoFree( autoFree )
    { }
};

struct VertexData : IDataAttachment
{
    uint32_t vertexCount;
};

struct IndexData : IDataAttachment
{
    uint32_t indexCount;
};

struct SamplerDataAttachment : IDataAttachment
{
    ResourceImageFormat format;
    uint32_t width { };
    uint32_t height { };
    uint32_t channels { };
    ECS::Material::TextureInfo textureInfo;

    SamplerDataAttachment( ) : IDataAttachment( false ) // Should not free contents
    { }
};

struct CubeMapDataAttachment : IDataAttachment
{
    std::vector< std::shared_ptr< SamplerDataAttachment > > images;
};

struct ShaderResource
{
    // Todo maybe the identifier should be required
    ResourceIdentifier identifier;
    ResourceType type;
    ResourceLoadStrategy loadStrategy;
    ResourceBindStrategy bindStrategy = ResourceBindStrategy::BindPerFrame;
    ResourcePersistStrategy persistStrategy;
    ResourceShaderStage shaderStage;

    std::shared_ptr< IDataAttachment > dataAttachment;
    std::function< void( ) > allocate;
    std::function< void( ) > update;
    std::function< void( const ResourceUsage &usage ) > prepareForUsage;
    std::function< void( ) > deallocate;

    void *apiSpecificBuffer;
};

class IResourceProvider
{
public:
    virtual std::shared_ptr< ShaderResource > createResource( const ShaderResourceRequest &request ) = 0;
    virtual std::shared_ptr< IResourceLock > createLock( const ResourceLockType &lockType ) = 0;
    virtual ~IResourceProvider( ) = default;
};

END_NAMESPACES
