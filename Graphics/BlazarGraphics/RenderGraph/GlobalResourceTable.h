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
#include "StaticVars.h"
#include "../IRenderDevice.h"
#include "../AssetManager.h"
#include "../DataAttachmentFormatter.h"
#include "Pass.h"
#include "ShaderUniformBinder.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct ShaderResourceWrapper
{
    bool isAllocated = false;
    std::shared_ptr< ShaderResource > ref;
};

struct GeometryData
{
    std::vector< bool > loadOnceResourcesAdded = { };
    std::vector< ShaderResourceWrapper > resources;
    std::vector< ShaderResourceWrapper > boundResources;
    SubMeshGeometry subMeshGeometry;
};

struct EntityWrapper
{
    MeshGeometry geometryRef;
    ECS::IGameEntity * entity;
    std::vector< GeometryData > subGeometries;
};

class GlobalResourceTable
{
private:
    AssetManager *assetManager;
    IRenderDevice *renderDevice;

    std::unique_ptr< ShaderUniformBinder > resourceBinder;
    std::unique_ptr< ECS::IGameEntity > dummy;

    ECS::ComponentTable * currentComponentTable;
    std::vector< std::vector< ShaderResourceWrapper > > frameResources;
    std::vector< std::vector< bool > > frameUpdatedResources;

    std::vector< EntityWrapper > geometryList;
    std::vector< EntityWrapper > quadGeometryList;
    std::vector< EntityWrapper > triangleGeometryList;
    std::vector< EntityWrapper > cubeGeometryList;

    std::vector< std::vector< uint32_t > > entityGeometryMap;

    std::vector< bool > bindersAssigned;
    std::vector< int > perFrameResources;
    std::vector< int > perEntityResources;
    std::vector< int > perGeometryResources;
public:
    explicit GlobalResourceTable( IRenderDevice *renderDevice, AssetManager *assetManager );

    void addEntity( ECS::IGameEntity* entity );
    void updateEntity( ECS::IGameEntity* entity );
    void removeEntity( ECS::IGameEntity* entity );

    void resetTable( ECS::ComponentTable * componentTable, const uint32_t &frameIndex );
    void resetFrame( const int& frameIdx );

    std::shared_ptr< ShaderResource >& getResource( const int& resourceIdx, const uint32_t &frameIndex );
    std::unique_ptr< SamplerDataAttachment > getSamplerDataAttachment( const ECS::Material::TextureInfo& texture );

    std::vector< EntityWrapper >& getGeometryList( const InputGeometry& inputGeometry );

    inline ShaderUniformBinder *getResourceBinder( ) { return resourceBinder.get( ); }

    void allocateAllPerGeometryResources( const int& frameIndex, const MeshGeometry &parent, const SubMeshGeometry &subMeshGeometry );
    void allocateAllPerEntityResources( const int& frameIndex, ECS::IGameEntity* entity );
    void allocatePerEntityResources( const int& frameIndex, ECS::IGameEntity* entity, std::vector< int > resources );
    void allocateAllPerFrameResources( const int& frameIndex );

    bool isBinderAssigned( const int& binderIdx );
    void addPerGeometryResource( const int& binderIdx );
    void addPerEntityResource( const int& binderIdx );
    void addPerFrameResource( const int& binderIdx );

    ~GlobalResourceTable( );
private:
    void allocateResource( const int &resourceIdx, const std::string& uniformName, const uint32_t &frameIndex, const IShaderUniform * content );

    std::shared_ptr< ShaderResource > createResource( const ResourceType &type = ResourceType::Uniform,
                                                      const ResourceLoadStrategy &loadStrategy = ResourceLoadStrategy::LoadPerFrame,
                                                      const ResourcePersistStrategy &persistStrategy = ResourcePersistStrategy::StoreOnDeviceMemory,
                                                      const ResourceShaderStage &shaderStage = ResourceShaderStage::Vertex ) const;

    EntityWrapper createGeometryData( ECS::IGameEntity* entity );
    void attachAllAttachments( const IShaderUniform * content, const std::shared_ptr<ShaderResource>& resource );
    void attachUniformAttachment( const IShaderUniform * content, const std::shared_ptr<ShaderResource>& resource ) const;
    void attachCubeMapAttachment( const IShaderUniform * content, const std::shared_ptr<ShaderResource>& resource ) const;
    void attachSamplerAttachment( const IShaderUniform * content, const std::shared_ptr<ShaderResource>& resource );
    void createGeometry( ECS::IGameEntity* entity );
    void createGeometryList( const std::vector< ECS::IGameEntity * > &entities );
    GeometryData createGeometryData( ECS::IGameEntity* entity, SubMeshGeometry &subMeshGeometry );

    static void cleanGeometryData( GeometryData &geometryData );
    static void freeResource( std::shared_ptr< ShaderResource >& resource );
};

END_NAMESPACES

