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

#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include "../AssetManager.h"

#include <utility>
#include <vector>
#include <optional>
#include "../IResourceProvider.h"
#include "../DataAttachmentFormatter.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum class UpdateFrequency
{
    EachGeometry,
    EachEntity,
    EachFrame,
    Once
};

enum class UniformType
{
    Sampler,
    Plain
};

class IShaderUniform
{
public:
    explicit IShaderUniform( UniformType uniformType ) : uniformType( uniformType )
    { };

    const UniformType uniformType;
    ResourceType resourceType = ResourceType::Uniform;
    UpdateFrequency updateFrequency = UpdateFrequency::EachGeometry;

    virtual ~IShaderUniform( ) = default;
};

class StructShaderUniform : public IShaderUniform
{
public:
    StructShaderUniform( ) : IShaderUniform( UniformType::Plain )
    { };

    uint32_t size { };
    void *data { };

    ~StructShaderUniform( ) override = default;
};

class SamplerShaderUniform : public IShaderUniform
{
public:
    SamplerShaderUniform( ) : IShaderUniform( UniformType::Sampler )
    { };

    std::vector< ECS::Material::TextureInfo > textures;

    ~SamplerShaderUniform( ) override = default;
};

typedef std::function< std::unique_ptr< IShaderUniform >( const MeshGeometry &parent, const SubMeshGeometry &subMeshGeometry ) > PerGeometryBinder;
typedef std::function< std::unique_ptr< IShaderUniform >( ECS::IGameEntity * entity ) > PerEntityUniformBinder;
typedef std::function< std::unique_ptr< IShaderUniform >( ECS::ComponentTable * components ) > PerFrameUniformBinder;

struct AllocatorFunction
{
    int refIdx;
    std::string refUniform;
    UpdateFrequency frequency;
    PerGeometryBinder perGeometryBinder;
    PerEntityUniformBinder perEntityUniformBinder;
    PerFrameUniformBinder perFrameUniformBinder;
};

class ShaderUniformBinder
{
private:
    std::vector< AllocatorFunction > binders;
    std::unordered_map< std::string, int > binderIdxMap;
    std::vector< AllocatorFunction > loadOnceBinders;
public:
    ShaderUniformBinder();

    void registerBinder( std::string uniformName, PerGeometryBinder binder );
    void registerBinder( std::string uniformName, PerEntityUniformBinder binder );
    void registerBinder( std::string uniformName, PerFrameUniformBinder binder );
    void registerBinderLoadOnce( std::string uniformName, PerEntityUniformBinder binder );

    [[nodiscard]] bool checkBinderExistsByIdx( const int& idx ) const
    {
        return idx < binders.size();
    }

    bool checkBinderExistsByName( const std::string& name )
    {
        return binderIdxMap.find( name ) != binderIdxMap.end( );
    }

    // Use when performance is not important
    AllocatorFunction getBinderByName( const std::string& name )
    {
        return binders[ getBinderIdx( name ) ];
    }

    // Use with performance critical code
    AllocatorFunction getBinderByIdx( const int& idx )
    {
        return binders[ idx ];
    }

    int getBinderIdx( const std::string& name )
    {
        return binderIdxMap[ name ];
    }

    [[nodiscard]] const std::vector< AllocatorFunction >& getAllLoadOnceAllocators( ) const
    {
        return loadOnceBinders;
    }
private:
    int registerBinder( std::string uniformName, AllocatorFunction allocator );

    template< class T >
    static std::unique_ptr< IShaderUniform > getAttachment( const T& data )
    {
        StructShaderUniform * result = new StructShaderUniform { };
        result->size = sizeof( T );
        result->data = static_cast< char* >( malloc( result->size ) );
        memcpy( result->data, &data, result->size );
        return std::unique_ptr< StructShaderUniform >( result );
    }

    static std::unique_ptr< IShaderUniform > createSamplerShaderUniform( const std::vector< ECS::Material::TextureInfo >& textures, const ResourceType& type = ResourceType::Sampler2D )
    {
        std::unique_ptr< SamplerShaderUniform > shaderUniform = std::make_unique< SamplerShaderUniform >( );
        shaderUniform->resourceType = type;
        shaderUniform->textures = textures;
        return std::move( shaderUniform );
    }
};

END_NAMESPACES

