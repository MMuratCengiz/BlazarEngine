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

#include <utility>
#include "../IResourceProvider.h"
#include "../DataAttachmentFormatter.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct UniformAttachmentContent
{
    char *data;
    uint32_t size;
    ResourceType resourceType;
};

struct TextureAttachmentContent
{
    std::vector< ECS::Material::TextureInfo > textures;
    ResourceType resourceType;
};

enum class ResourceBindType
{
    PerFrameUniform,
    PerEntityUniform,
    PerFrameTexture,
    PerEntityTexture
};

typedef std::function< UniformAttachmentContent( const std::shared_ptr< ECS::ComponentTable > &components ) > PerFrameUniformBinder;
typedef std::function< UniformAttachmentContent( const std::shared_ptr< ECS::IGameEntity > &entity ) > PerEntityUniformBinder;
typedef std::function< TextureAttachmentContent( const std::shared_ptr< ECS::ComponentTable > &components ) > PerFrameTextureBinder;
typedef std::function< TextureAttachmentContent( const std::shared_ptr< ECS::IGameEntity > &entity ) > PerEntityTextureBinder;

struct ResourceBinderFunc
{
    PerFrameUniformBinder perFrameUniformBinder;
    PerEntityUniformBinder perEntityUniformBinder;
    PerFrameTextureBinder perFrameTextureBinder;
    PerEntityTextureBinder perEntityTextureBinder;

    explicit ResourceBinderFunc( PerFrameUniformBinder binder )
    {
        this->perFrameUniformBinder = std::move( binder );
    }

    explicit ResourceBinderFunc( PerEntityUniformBinder binder )
    {
        this->perEntityUniformBinder = std::move( binder );
    }

    explicit ResourceBinderFunc( PerFrameTextureBinder binder )
    {
        this->perFrameTextureBinder = std::move( binder );
    }

    explicit ResourceBinderFunc( PerEntityTextureBinder binder )
    {
        this->perEntityTextureBinder = std::move( binder );
    }

    ResourceBinderFunc &operator=( PerFrameUniformBinder binder )
    {
        this->perFrameUniformBinder = std::move( binder );
        return *this;
    }

    ResourceBinderFunc &operator=( PerEntityUniformBinder binder )
    {
        this->perEntityUniformBinder = std::move( binder );
        return *this;
    }

    ResourceBinderFunc &operator=( PerFrameTextureBinder binder )
    {
        this->perFrameTextureBinder = std::move( binder );
        return *this;
    }

    ResourceBinderFunc &operator=( PerEntityTextureBinder binder )
    {
        this->perEntityTextureBinder = std::move( binder );
        return *this;
    }

    ~ResourceBinderFunc( ) { };
};

class ResourceBinder
{
private:
    std::unordered_map< std::string, ResourceBindStrategy > resourceBindStrategies;
    std::unordered_map< std::string, ResourceBindType > resourceBindTypes;
    std::unordered_map< std::string, ResourceType > resourceTypes;
    std::unordered_map< std::string, PerFrameUniformBinder > perFrameUniformBinders;
    std::unordered_map< std::string, PerEntityUniformBinder > perEntityUniformBinders;
    std::unordered_map< std::string, PerFrameTextureBinder > perFrameTextureBinders;
    std::unordered_map< std::string, PerEntityTextureBinder > perEntityTextureBinders;

public:
    ResourceBinder( );
    void registerResourceBinder( const std::string &resourceName, const ResourceBindType &bindType, const ResourceType &resourceType, ResourceBinderFunc binder );
    ResourceBindStrategy getResourceBindStrategy( const std::string &resourceName ) const;
    ResourceBindType getResourceBindType( const std::string &resourceName ) const;
    std::optional< ResourceBindType > getResourceBindTypeOptional( const std::string &resourceName ) const;
    ResourceType getResourceType( const std::string &resourceName ) const;
    PerFrameUniformBinder getResourcePerFrameUniformBinder( const std::string &resourceName ) const;
    PerEntityUniformBinder getResourcePerEntityUniformBinder( const std::string &resourceName ) const;
    PerFrameTextureBinder getResourcePerFrameTextureBinder( const std::string &resourceName ) const;
    PerEntityTextureBinder getResourcePerEntityTextureBinder( const std::string &resourceName ) const;

    std::vector< std::string > getAllPerEntityBinders(  ) const;
};

END_NAMESPACES