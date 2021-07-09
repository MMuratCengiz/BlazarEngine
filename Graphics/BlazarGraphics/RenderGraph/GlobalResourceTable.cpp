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

GlobalResourceTable::GlobalResourceTable( IRenderDevice* renderDevice, AssetManager* assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
	frameResources.resize( renderDevice->getFrameCount( ) );

	std::shared_ptr< ECS::IGameEntity > dummy = std::make_shared< ECS::DynamicGameEntity >( );

	resourceBinder = std::make_unique< ShaderUniformBinder >( );

	auto createPrimitiveEntityWrapper = [ = ]( const PrimitiveType& type )
	{
		auto primitive = assetManager->getPrimitive( type );
		SubMeshGeometry& subGeometry = primitive.subGeometries[ 0 ];

		EntityWrapper entityWrapper = { };
		entityWrapper.entity = dummy;
		entityWrapper.geometryRef = assetManager->getPrimitive( type );
		entityWrapper.subGeometries.push_back( createGeometryData( dummy, subGeometry ) );

		return entityWrapper;
	};

	quadGeometryList.push_back( std::move( createPrimitiveEntityWrapper( PrimitiveType::PlainSquare ) ) );
	cubeGeometryList.push_back( std::move( createPrimitiveEntityWrapper( PrimitiveType::PlainCube ) ) );
	triangleGeometryList.push_back( std::move( createPrimitiveEntityWrapper( PrimitiveType::PlainTriangle ) ) );
}

void GlobalResourceTable::resetTable( const std::shared_ptr< ECS::ComponentTable >& componentTable, const uint32_t& frameIndex )
{
	currentComponentTable = componentTable;
}

void GlobalResourceTable::resetFrame( const int& frameIdx )
{
    // std::fill( frameUpdatedResources[ frameIdx ].begin(  ), frameUpdatedResources[ frameIdx ].end( ), false );    
}

void GlobalResourceTable::addEntity( const std::shared_ptr< ECS::IGameEntity >& entity )
{
	createGeometry( entity );
	createGeometryList( entity->getChildren( ) );
}

void GlobalResourceTable::updateEntity( const std::shared_ptr< ECS::IGameEntity >& entity )
{
	removeEntity( entity );
	addEntity( entity );
}

void GlobalResourceTable::removeEntity( const std::shared_ptr< ECS::IGameEntity >& entity )
{
	FUNCTION_BREAK( entity->getUID( ) >= entityGeometryMap.size( ) );

	for ( int idx : entityGeometryMap[ entity->getUID( ) ] )
	{
		auto& geometryData = geometryList[ idx ];

		for ( auto& subGeometry : geometryData.subGeometries )
		{
			cleanGeometryData( subGeometry );
		}
	}
}

void GlobalResourceTable::createGeometry( const std::shared_ptr< ECS::IGameEntity >& entity )
{
	FUNCTION_BREAK( !entity->hasComponent< ECS::CMesh >( ) );
	FUNCTION_BREAK( entity->hasComponent< ECS::CCubeMap >( ) );

	const auto& entityGeometries = createGeometryData( entity );

	geometryList.push_back( entityGeometries );

	if ( entity->getUID( ) >= entityGeometryMap.size( ) )
	{
		entityGeometryMap.resize( entity->getUID( ) + 1 );
	}

	entityGeometryMap[ entity->getUID( ) ] = { };

	for ( uint32_t i = geometryList.size( ) - entityGeometries.subGeometries.size( ); i < geometryList.size( ); ++i )
	{
		entityGeometryMap[ entity->getUID( ) ].push_back( i );
	}
}

EntityWrapper GlobalResourceTable::createGeometryData( const std::shared_ptr< ECS::IGameEntity >& entity )
{
	auto transformComponent = entity->getComponent< ECS::CTransform >( );
	const auto meshComponent = entity->getComponent< ECS::CMesh >( );
	auto materialComponent = entity->getComponent< ECS::CMaterial >( );
	auto tessellationComponent = entity->getComponent< ECS::CTessellation >( );

	std::string parentBoundingName = StaticVars::getInputName( StaticVars::Input::GeometryData );

	MeshGeometry geometry = assetManager->getMeshGeometry( meshComponent->geometryRefIdx );

	EntityWrapper result = { };
	result.entity = entity;

	for ( SubMeshGeometry& subMeshGeometry : geometry.subGeometries )
	{
		result.subGeometries.push_back( std::move( createGeometryData( entity, subMeshGeometry ) ) );
	}

	result.geometryRef = std::move( geometry );

	return std::move( result );
}

GeometryData GlobalResourceTable::createGeometryData( const std::shared_ptr< ECS::IGameEntity >& entity, SubMeshGeometry& subMeshGeometry )
{
	GeometryData data{ };
	data.subMeshGeometry = subMeshGeometry;

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
	vertices->allocate( );

	data.resources.push_back( { true, std::move( vertices ) } );
	// ---

	/*
	 * Create index data:
	 */
	if ( !subMeshGeometry.indices.empty( ) )
	{
		auto indices = createResource( ResourceType::IndexData, ResourceLoadStrategy::LoadOnce );
		indices->identifier = { "IndexData" };
		std::shared_ptr< IndexData > indexData = std::make_shared< IndexData >( );
		indexData->indexCount = subMeshGeometry.indices.size( );
		indexData->content = subMeshGeometry.indices.data( );
		indexData->size = subMeshGeometry.indices.size( ) * sizeof( uint32_t );
		indices->dataAttachment = std::move( indexData );
		indices->allocate( );

		data.resources.push_back( { true, std::move( indices ) } );
	}

	for ( const auto& loadOnceBinder : resourceBinder->getAllLoadOnceAllocators( ) )
	{
		const auto uContent = loadOnceBinder.perEntityUniformBinder( entity );
		const auto content = uContent.get( );

		auto resource = createResource( content->resourceType );
		resource->identifier = { loadOnceBinder.refUniform };

		attachAllAttachments( content, resource );

		resource->allocate( );

		if ( data.boundResources.size( ) <= loadOnceBinder.refIdx )
		{
			data.boundResources.resize( loadOnceBinder.refIdx + 1 );
		}

		data.boundResources[ loadOnceBinder.refIdx ] = { true, resource };
	}

	return data;
}

void GlobalResourceTable::attachAllAttachments( const IShaderUniform* content, std::shared_ptr<ShaderResource> resource )
{
	attachUniformAttachment( content, resource );
	attachCubeMapAttachment( content, resource );
	attachSamplerAttachment( content, resource );
}

void GlobalResourceTable::attachUniformAttachment( const IShaderUniform* content, std::shared_ptr<ShaderResource> resource ) const
{
	if ( content->resourceType == ResourceType::Uniform || content->resourceType == ResourceType::PushConstant )
	{
		if ( resource->dataAttachment == nullptr )
		{
			resource->dataAttachment = std::make_shared< IDataAttachment >( );
		}

	    free( resource->dataAttachment->content );

		const auto* pUniform = dynamic_cast< const StructShaderUniform* >( content );

		resource->dataAttachment->size = pUniform->size;
		resource->dataAttachment->content = pUniform->data;
	}
}

void GlobalResourceTable::attachCubeMapAttachment( const IShaderUniform* content, std::shared_ptr<ShaderResource> resource ) const
{
	if ( content->resourceType == ResourceType::CubeMap )
	{
		std::shared_ptr< CubeMapDataAttachment > cubeMapAttachment = std::make_shared< CubeMapDataAttachment >( );

		const auto* pUniform = dynamic_cast< const SamplerShaderUniform* >( content );

		for ( const auto& texture : pUniform->textures )
		{
			cubeMapAttachment->images.push_back( assetManager->getImage( texture.path ) );
		}

		resource->dataAttachment = std::move( cubeMapAttachment );
	}
}


void GlobalResourceTable::attachSamplerAttachment( const IShaderUniform* content, std::shared_ptr<ShaderResource> resource )
{
	if ( content->resourceType == ResourceType::Sampler2D )
	{
		const auto* pUniform = dynamic_cast< const SamplerShaderUniform* >( content );

		auto samplerAttachment = pUniform->textures.empty( ) ? nullptr : getSamplerDataAttachment( pUniform->textures[ 0 ] );

		resource->dataAttachment = std::move( samplerAttachment );
	}
}

void GlobalResourceTable::createGeometryList( const std::vector< std::shared_ptr< ECS::IGameEntity > >& entities )
{
	for ( const auto& entity : entities )
	{
		createGeometry( entity );
		createGeometryList( entity->getChildren( ) );
	}
}

std::shared_ptr< ShaderResource > GlobalResourceTable::createResource( const ResourceType& type,
	const ResourceLoadStrategy& loadStrategy,
	const ResourcePersistStrategy& persistStrategy,
	const ResourceShaderStage& shaderStage ) const
{
	ShaderResourceRequest shaderResourceRequest{ };
	shaderResourceRequest.type = type;
	shaderResourceRequest.loadStrategy = loadStrategy;
	shaderResourceRequest.persistStrategy = persistStrategy;
	shaderResourceRequest.shaderStage = shaderStage;

	return renderDevice->getResourceProvider( )->createResource( shaderResourceRequest );
}

void GlobalResourceTable::allocateResource( const int& resourceIdx, const std::string& uniformName, const uint32_t& frameIndex, const IShaderUniform* content )
{
	if ( resourceIdx >= frameResources[ frameIndex ].size( ) )
	{
		frameResources[ frameIndex ].resize( resourceIdx + 1 );
		frameResources[ frameIndex ][ resourceIdx ] = { };
	}

	auto& wrapper = frameResources[ frameIndex ][ resourceIdx ];

	const bool wrapperAllocatedThisFrame = !wrapper.isAllocated;

	if ( wrapperAllocatedThisFrame )
	{
		wrapper.isAllocated = true;
		wrapper.ref = createResource( content->resourceType );
		wrapper.ref->identifier = { uniformName };
		wrapper.ref->type = content->resourceType;
	}
	else
	{
		FUNCTION_BREAK( wrapper.ref->loadStrategy == ResourceLoadStrategy::LoadOnce )
	}

	attachAllAttachments( content, wrapper.ref );

	FUNCTION_BREAK( wrapper.ref->type == ResourceType::PushConstant );

	if ( wrapperAllocatedThisFrame )
	{
		wrapper.ref->allocate( );
	}
	else
	{
		wrapper.ref->update( );
	}
}

auto GlobalResourceTable::getSamplerDataAttachment( const ECS::Material::TextureInfo& texture ) -> std::shared_ptr< SamplerDataAttachment >&
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

std::shared_ptr< ShaderResource >& GlobalResourceTable::getResource( const int& resourceIdx, const uint32_t& frameIndex )
{
	auto resource = frameResources[ frameIndex ][ resourceIdx ];
	return resource.ref;
}

void GlobalResourceTable::allocateAllPerGeometryResources( const int& frameIndex, const MeshGeometry& parent, const SubMeshGeometry& subMeshGeometry )
{
	for ( const int& binderIdx : perGeometryResources )
	{
		auto binder = resourceBinder->getBinderByIdx( binderIdx );
		auto content = binder.perGeometryBinder( parent, subMeshGeometry );
		allocateResource( binderIdx, binder.refUniform, frameIndex, content.get( ) );
	}
}

void GlobalResourceTable::allocateAllPerEntityResources( const int& frameIndex, const std::shared_ptr< ECS::IGameEntity >& entity )
{
	for ( const int& binderIdx : perEntityResources )
	{
		auto binder = resourceBinder->getBinderByIdx( binderIdx );
		auto content = binder.perEntityUniformBinder( entity );
		allocateResource( binderIdx, binder.refUniform, frameIndex, content.get( ) );
	}
}

void GlobalResourceTable::allocatePerEntityResources( const int& frameIndex, const std::shared_ptr< ECS::IGameEntity >& entity, std::vector< int > resources )
{
	for ( const int& binderIdx : resources )
	{
		auto binder = resourceBinder->getBinderByIdx( binderIdx );
		auto content = binder.perEntityUniformBinder( entity );
		allocateResource( binderIdx, binder.refUniform, frameIndex, content.get( ) );
	}
}

void GlobalResourceTable::allocateAllPerFrameResources( const int& frameIndex )
{
	for ( const int& binderIdx : perFrameResources )
	{
		auto binder = resourceBinder->getBinderByIdx( binderIdx );
		auto content = binder.perFrameUniformBinder( currentComponentTable );
		allocateResource( binderIdx, binder.refUniform, frameIndex, content.get( ) );
	}
}

bool GlobalResourceTable::isBinderAssigned( const int& binderIdx )
{
	if ( binderIdx >= bindersAssigned.size( ) )
	{
		bindersAssigned.resize( 1 + binderIdx, false );
		bindersAssigned[ binderIdx ] = true;
		return false;
	}

	return bindersAssigned[ binderIdx ];
}
void GlobalResourceTable::addPerGeometryResource( const int& binderIdx )
{
	if ( !isBinderAssigned( binderIdx ) )
	{
		perGeometryResources.push_back( binderIdx );
		bindersAssigned[ binderIdx ] = true;
	}
}

void GlobalResourceTable::addPerEntityResource( const int& binderIdx )
{
	if ( !isBinderAssigned( binderIdx ) )
	{
		perEntityResources.push_back( binderIdx );
		bindersAssigned[ binderIdx ] = true;
	}
}

void GlobalResourceTable::addPerFrameResource( const int& binderIdx )
{
	if ( !isBinderAssigned( binderIdx ) )
	{
		perFrameResources.push_back( binderIdx );
		bindersAssigned[ binderIdx ] = true;
	}
}

std::vector< EntityWrapper >& GlobalResourceTable::getGeometryList( const InputGeometry& inputGeometry )
{
	switch ( inputGeometry )
	{
	case InputGeometry::Model:
		break;
	case InputGeometry::Quad:
		return quadGeometryList;
	case InputGeometry::Cube:
		return cubeGeometryList;
	case InputGeometry::OverSizedTriangle:
		return triangleGeometryList;
	}

	return geometryList;
}

GlobalResourceTable::~GlobalResourceTable( )
{
	for ( uint32_t frameIndex = 0; frameIndex < renderDevice->getFrameCount( ); ++frameIndex )
	{
		for ( auto it = frameResources[ frameIndex ].begin( ); it != frameResources[ frameIndex ].end( ); )
		{
			if ( ShaderResourceWrapper resourceWrapper = *it; resourceWrapper.isAllocated )
			{
				freeResource( resourceWrapper.ref );

				it = frameResources[ frameIndex ].erase( it );
			}
			else
			{
				++it;
			}
		}
	}

	auto cleanGeometryDataList = [ ]( std::vector< EntityWrapper >& geometries )
	{
		for ( auto& geometry : geometries )
		{
			for ( auto& subGeometry : geometry.subGeometries )
			{
				cleanGeometryData( subGeometry );
			}
		}
	};

	cleanGeometryDataList( geometryList );
	cleanGeometryDataList( quadGeometryList );
	cleanGeometryDataList( triangleGeometryList );
	cleanGeometryDataList( cubeGeometryList );
}

void GlobalResourceTable::freeResource( std::shared_ptr< ShaderResource >& resource )
{
	FUNCTION_BREAK( resource == nullptr );

	if ( resource->type != ResourceType::PushConstant )
	{
		resource->deallocate( );
	}

	FUNCTION_BREAK( resource->dataAttachment == nullptr );
	FUNCTION_BREAK( resource->dataAttachment->content == nullptr );
	FUNCTION_BREAK( resource->dataAttachment->size == 0 );
	FUNCTION_BREAK( resource->dataAttachment->autoFree );

	free( resource->dataAttachment->content );
}

void GlobalResourceTable::cleanGeometryData( GeometryData& geometryData )
{
	for ( auto& resource : geometryData.resources )
	{
		freeResource( resource.ref );
	}

	for ( auto& resource : geometryData.boundResources )
	{
		freeResource( resource.ref );
	}
}

END_NAMESPACES