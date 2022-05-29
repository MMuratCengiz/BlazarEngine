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

#include <BlazarGraphics/AssetManager.h>
#include <BlazarGraphics/GraphicsException.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <boost/format.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

AssetManager::AssetManager( )
{
	geometryTable.resize( 4 );
	const std::string& litCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::LitCube );
	geometryTable[ LIT_CUBE_GEOMETRY_IDX ] = { };
	MeshGeometry& litCube = geometryTable[ LIT_CUBE_GEOMETRY_IDX ];
	SubMeshGeometry& litCubeSubMesh = litCube.subGeometries.emplace_back( );
	litCubeSubMesh.dataRaw = litCubePrimitive.getData( );
	litCubeSubMesh.vertexCount = litCubePrimitive.getVertexCount( );

	const std::string& plainCubePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
	geometryTable[ PLAIN_CUBE_GEOMETRY_IDX ] = { };
	MeshGeometry& plainCube = geometryTable[ PLAIN_CUBE_GEOMETRY_IDX ];
	SubMeshGeometry& plainCubeSubMesh = plainCube.subGeometries.emplace_back( );
	plainCubeSubMesh.dataRaw = plainCubePrimitive.getData( );
	plainCubeSubMesh.vertexCount = plainCubePrimitive.getVertexCount( );

	const std::string& plainSquarePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );
	geometryTable[ PLAIN_SQUARE_GEOMETRY_IDX ] = { };
	MeshGeometry& plainSquare = geometryTable[ PLAIN_SQUARE_GEOMETRY_IDX ];
	SubMeshGeometry& plainSquareSubMesh = plainSquare.subGeometries.emplace_back( );
	plainSquareSubMesh.dataRaw = plainSquarePrimitive.getData( );
	plainSquareSubMesh.vertexCount = plainSquarePrimitive.getVertexCount( );

	const std::string& plainTrianglePath = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle );
	geometryTable[ PLAIN_TRIANGLE_GEOMETRY_IDX ] = { };
	MeshGeometry& plainTriangle = geometryTable[ PLAIN_TRIANGLE_GEOMETRY_IDX ];
	SubMeshGeometry& plainTriangleSubMesh = plainTriangle.subGeometries.emplace_back( );
	plainTriangleSubMesh.dataRaw = plainTrianglePrimitive.getData( );
	plainTriangleSubMesh.vertexCount = plainTrianglePrimitive.getVertexCount( );
}

std::unique_ptr< ECS::IGameEntity > AssetManager::createEntity( const std::string& meshPath )
{
	std::unique_ptr< ECS::IGameEntity > rootEntity = std::make_unique< ECS::DynamicGameEntity >( );

	createEntity( rootEntity.get( ), meshPath );

	return std::move( rootEntity );
}

void AssetManager::createEntity( ECS::IGameEntity* attachToEntity, const std::string& meshPath )
{
	if ( meshPath == Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LitCube ) )
	{
		attachToEntity->createComponent< ECS::CMaterial >( );

		auto mesh = attachToEntity->createComponent< ECS::CMesh >( );
		mesh->path = meshPath;
		mesh->geometryRefIdx = LIT_CUBE_GEOMETRY_IDX;
	}
	else if ( meshPath == Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::PlainCube ) )
	{
		attachToEntity->createComponent< ECS::CMaterial >( );

		auto mesh = attachToEntity->createComponent< ECS::CMesh >( );
		mesh->path = meshPath;
		mesh->geometryRefIdx = PLAIN_CUBE_GEOMETRY_IDX;
	}
	else if ( meshPath == Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::PlainSquare ) )
	{
		attachToEntity->createComponent< ECS::CMaterial >( );

		auto mesh = attachToEntity->createComponent< ECS::CMesh >( );
		mesh->path = meshPath;
		mesh->geometryRefIdx = PLAIN_SQUARE_GEOMETRY_IDX;
	}
	else if ( meshPath == Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::PlainTriangle ) )
	{
		attachToEntity->createComponent< ECS::CMaterial >( );

		auto mesh = attachToEntity->createComponent< ECS::CMesh >( );
		mesh->path = meshPath;
		mesh->geometryRefIdx = PLAIN_TRIANGLE_GEOMETRY_IDX;
	}
	else
	{
		loadModel( attachToEntity, meshPath );
	}
}

void AssetManager::loadImage( const std::string& path )
{
	int width, height, channels;

	stbi_uc* contents = stbi_load( path.c_str( ), &width, &height, &channels, STBI_rgb_alpha );

	if ( contents == nullptr )
	{
		Core::Logger::get( ).log( Core::Verbosity::Debug, "AssetManager", stbi_failure_reason( ) );

		throw std::runtime_error( "Couldn't find texture." );
	}

	imageMap[ path ] = std::make_unique< SamplerDataAttachment >( );
	auto& ptr = imageMap[ path ];
	ptr->content = contents;
	ptr->width = static_cast< uint32_t >( width );
	ptr->height = static_cast< uint32_t >( height );
	ptr->channels = static_cast< uint32_t >( channels );
}

void AssetManager::loadModel( ECS::IGameEntity* rootEntity, const std::string& path )
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	SceneContext context{ };
	context.rootEntity = rootEntity;
	context.gltfModelDirectory = Core::Utilities::getFileDirectory( path );
	context.nodeTree = std::shared_ptr< Core::SimpleTree< MeshNode > >( new Core::SimpleTree< MeshNode >{ }, [ = ]( Core::SimpleTree< MeshNode >* p )
		{
			p->freeNode( p->getRoot( ) );
			delete p;
		} );

	bool res = loader.LoadASCIIFromFile( &context.model, &err, &warn, path );

	if ( !res )
	{
		std::stringstream ss;
		ss << "ERROR::ASSIMP::" << err;
		throw std::runtime_error( ss.str( ) );
	}

	generateAnimationData( context );

	MeshNode rootNode = { };
	rootNode.rotation = glm::quat( glm::mat4( 1.0f ) );
	rootNode.translation = glm::vec3( 0.0f );
	rootNode.scale = glm::vec3( 1.0f );

	context.nodeTree->setRootData( 0, rootNode );

	for ( const tinygltf::Scene& scene : context.model.scenes )
	{
		context.multiMeshNodes = context.model.meshes.size( ) > 1;

		// Attach the initial tree structure to the root node because gltf doesn't do it by default
		for ( int node : scene.nodes )
		{
			onEachNode( context, rootEntity, path, -1, node );
		}
	}

	for ( auto node : context.nodeTree->flattenTree( true ) )
	{
		tinygltf::Node tNode = context.model.nodes[ node->id ];

		if ( tNode.mesh != -1 )
		{
			onEachMesh( context, path, node );
		}
	}
}

void AssetManager::generateAnimationData( SceneContext& context )
{
	const tinygltf::Model& model = context.model;

	for ( const tinygltf::Animation& animation : model.animations )
	{
		AnimationData animationData = { };

		for ( const tinygltf::AnimationChannel& channel : animation.channels )
		{
			onEachChannel( model, animation, animationData, channel );
		}

		context.animations[ animation.name ] = std::move( animationData );
	}
}

void AssetManager::onEachChannel( const tinygltf::Model& model, const tinygltf::Animation& animation, AnimationData& animationData, const tinygltf::AnimationChannel& channel )
{
	AnimationChannel animationChannel = { };

	animationChannel.targetJoint = channel.target_node;

	const tinygltf::AnimationSampler& sampler = animation.samplers[ channel.sampler ];

	std::string targetPath = channel.target_path;
	boost::to_lower( targetPath );

	std::string interpolationType = sampler.interpolation;
	boost::to_lower( interpolationType );

	animationChannel.transformType =
		targetPath == "translation" ? Translation :
		targetPath == "rotation" ? Rotation :
		targetPath == "scale" ? Scale :
		Weights;

	animationChannel.interpolationType =
		interpolationType == "linear" ? Linear :
		interpolationType == "step" ? Step :
		CubicSpline;

	std::vector< float > transformFlat;

	copyAccessorToVectorTransformed( animationChannel.keyFrames, model, sampler.input );
	copyAccessorToVectorTransformed( transformFlat, model, sampler.output );

	tinygltf::Accessor accessor = model.accessors[ sampler.output ];

	for ( int i = 0; i < animationChannel.keyFrames.size( ); ++i )
	{
		switch ( accessor.type )
		{
		case TINYGLTF_TYPE_VEC3:
		{
			glm::vec3 data = glm::make_vec3( transformFlat.data( ) + ( i * 3 ) );
			animationChannel.transform.emplace_back( data, 0.0f );
			break;
		}
		case TINYGLTF_TYPE_VEC4:
		{
			glm::vec4 data = glm::make_vec4( transformFlat.data( ) + ( i * 4 ) );
			animationChannel.transform.push_back( data );
			break;
		}
		}
	}

	animationData.channels.push_back( std::move( animationChannel ) );
}

void AssetManager::onEachNode( SceneContext& context, ECS::IGameEntity* rootEntity, const std::string& currentRootPath, const int& parentNode, const int& currentNode )
{
	addNode( context, parentNode, currentNode );

	const tinygltf::Model& model = context.model;
	tinygltf::Node node = model.nodes[ currentNode ];

	auto entity = rootEntity;

	if ( node.mesh != -1 )
	{
		if ( context.multiMeshNodes )
		{
			std::unique_ptr< ECS::IGameEntity > child = std::make_unique< ECS::DynamicGameEntity >( );
			entity->addManagedChild( std::move( child ) );
			auto children = rootEntity->getChildren( );
			entity = children[ children.size( ) - 1 ];
		}

		std::ostringstream keyBuilder;
		keyBuilder << currentRootPath << "#" << node.name;

		geometryTable.push_back( { } );

		MeshContext meshContext = { };

		meshContext.geometryIdx = geometryTable.size( ) - 1;
		meshContext.meshNodeIdx = currentNode;

		entity->createComponent< ECS::CMaterial >( );
		entity->createComponent< ECS::CMesh >( );
		entity->getComponent< ECS::CMesh >( )->path = keyBuilder.str( );
		entity->getComponent< ECS::CMesh >( )->geometryRefIdx = geometryTable.size( ) - 1;;

		attachMaterialData( context, entity, node.mesh );

		if ( !context.model.animations.empty( ) )
		{
			auto animState = entity->createComponent< ECS::CAnimState >( );
			animState->mesh = entity->getComponent< ECS::CMesh >( );

			animState->boneTransformations.resize( model.skins[ node.skin ].joints.size( ), glm::mat4( 1.0f ) );
		}

		context.meshContextMap[ node.mesh ] = meshContext;
	}

	for ( int nodeIdx : model.nodes[ currentNode ].children )
	{
		onEachNode( context, entity, currentRootPath, currentNode, nodeIdx );
	}
}

void AssetManager::onEachMesh( SceneContext& context, const std::string& currentRootPath, Core::TreeNode< MeshNode >* treeNode )
{
	const tinygltf::Model& model = context.model;

	tinygltf::Node node = model.nodes[ treeNode->id ];

	generateMeshData( context, node.mesh );

	MeshContext meshContext = context.meshContextMap[ node.mesh ];

	MeshGeometry& geometry = geometryTable[ meshContext.geometryIdx ];

	if ( node.skin >= 0 )
	{
		onEachSkin( context, node.mesh, node.skin );
	}
}

void AssetManager::generateMeshData( SceneContext& context, const int& meshIdx )
{
	const tinygltf::Model& model = context.model;

	MeshContext meshContext = context.meshContextMap[ meshIdx ];

	MeshGeometry& geometry = geometryTable[ meshContext.geometryIdx ];

	geometry.nodeTree = context.nodeTree;
	geometry.meshNodeIdx = meshContext.meshNodeIdx;
	geometry.animations = context.animations;

	tinygltf::Mesh mesh = model.meshes[ meshIdx ];

	for ( const tinygltf::Primitive& primitive : mesh.primitives )
	{
		SubMeshGeometry& subMeshGeometry = geometry.subGeometries.emplace_back( );

		copyAccessorToVectorTransformed(
			subMeshGeometry.vertices, model, tryGetPrimitiveAttribute( primitive, "POSITION" )
		);

		copyAccessorToVectorTransformed(
			subMeshGeometry.normals, model, tryGetPrimitiveAttribute( primitive, "NORMAL" )
		);

		copyAccessorToVectorTransformed(
			subMeshGeometry.textureCoordinates, model, tryGetPrimitiveAttribute( primitive, "TEXCOORD_0" )
		);

		copyAccessorToVectorTransformed(
			subMeshGeometry.indices, model, primitive.indices
		);

		copyAccessorToVectorTransformed(
			subMeshGeometry.boneIndices, model, tryGetPrimitiveAttribute( primitive, "JOINTS_0" )
		);

		copyAccessorToVectorTransformed(
			subMeshGeometry.boneWeights, model, tryGetPrimitiveAttribute( primitive, "WEIGHTS_0" )
		);

		generateNormals( subMeshGeometry );

		subMeshGeometry.vertexCount = subMeshGeometry.vertices.size( ) / 3;
		subMeshGeometry.drawMode = primitive.mode == 0 ? PrimitiveDrawMode::Point : PrimitiveDrawMode::Triangle;

		packSubGeometry( subMeshGeometry );
	}
}

void AssetManager::generateNormals( SubMeshGeometry& subMeshGeometry ) const
{
	if ( subMeshGeometry.normals.empty( ) && subMeshGeometry.indices.empty( ) )
	{
		for ( int i = 0; i < subMeshGeometry.vertices.size( ); i += 9 )
		{
			glm::vec3 A = glm::make_vec3( &subMeshGeometry.vertices[ i ] );
			glm::vec3 B = glm::make_vec3( &subMeshGeometry.vertices[ i + 3 ] );
			glm::vec3 C = glm::make_vec3( &subMeshGeometry.vertices[ i + 6 ] );

			glm::vec3 normal = glm::normalize( glm::cross( B - A, C - A ) );

			for ( int j = 0; j < 9; ++j )
			{
				subMeshGeometry.normals.push_back( normal[ j % 3 ] );
			}
		}
	}

	if ( subMeshGeometry.normals.empty( ) && !subMeshGeometry.indices.empty( ) )
	{
		for ( int i = 0; i < subMeshGeometry.indices.size( ); i += 3 )
		{
			glm::vec3 A = glm::make_vec3( &subMeshGeometry.vertices[ subMeshGeometry.indices[ i ] ] );
			glm::vec3 B = glm::make_vec3( &subMeshGeometry.vertices[ subMeshGeometry.indices[ i + 1 ] ] );
			glm::vec3 C = glm::make_vec3( &subMeshGeometry.vertices[ subMeshGeometry.indices[ i + 2 ] ] );

			glm::vec3 normal = glm::normalize( glm::cross( B - A, C - A ) );

			for ( int j = 0; j < 9; ++j )
			{
				subMeshGeometry.normals.push_back( normal[ j % 3 ] );
			}
		}
	}
}

void AssetManager::onEachSkin( SceneContext& context, const int& meshIdx, const int& skinIdx )
{
	MeshContext meshContext = context.meshContextMap[ meshIdx ];

	const tinygltf::Model& model = context.model;

	tinygltf::Skin skin = model.skins[ skinIdx ];

	std::vector< float > inverseBindMatricesFlat;

	std::vector< glm::mat4 > ibmT;

	copyAccessorToVectorTransformed( inverseBindMatricesFlat, context.model, skin.inverseBindMatrices );

	MeshGeometry& geometry = geometryTable[ meshContext.geometryIdx ];

	geometry.joints = skin.joints;

	for ( int i = 0; i < skin.joints.size( ); i++ )
	{
		int joint = skin.joints[ i ];

		auto jointNode = geometry.nodeTree->findNode( joint );
		jointNode->data.inverseBindMatrix = flatMatToGLMMat( inverseBindMatricesFlat, i * 16 );
	}
}

void AssetManager::packSubGeometry( SubMeshGeometry& geometry )
{
	auto pushToDataRaw = [ & ]( const std::vector< float >& source, const int& offset, const int& count ) -> void
	{
		if ( source.empty( ) )
		{
			return;
		}

		for ( int i = 0; i < count; ++i )
		{
			geometry.dataRaw.push_back( ( float ) source[ offset + i ] );
		}
	};

	for ( int i = 0, texIdx = 0, boneIdx = 0; i < geometry.vertexCount * 3; i += 3, texIdx += 2, boneIdx += 4 )
	{
		pushToDataRaw( geometry.vertices, i, 3 ); geometry.dataRaw.push_back( 1.0f );
		pushToDataRaw( geometry.normals, i, 3 ); geometry.dataRaw.push_back( 0.0f );

		pushToDataRaw( geometry.textureCoordinates, texIdx, 2 );
		pushToDataRaw( geometry.boneIndices, boneIdx, 4 );
		pushToDataRaw( geometry.boneWeights, boneIdx, 4 );
	}
}

MeshGeometry& AssetManager::getMeshGeometry( const int& geometryIdx )
{
	int idx = geometryIdx;

	if ( idx == -1 )
	{
		throw GraphicsException{ "AssetManager", "Could not find geometry!" };
	}

	return geometryTable[ idx ];
}

MeshGeometry& AssetManager::getPrimitive( const PrimitiveType& primitive )
{
	switch ( primitive )
	{
	case PrimitiveType::LitCube:
		return geometryTable[ LIT_CUBE_GEOMETRY_IDX ];
	case PrimitiveType::PlainCube:
		return geometryTable[ PLAIN_CUBE_GEOMETRY_IDX ];
	case PrimitiveType::PlainSquare:
		return geometryTable[ PLAIN_SQUARE_GEOMETRY_IDX ];
	case PrimitiveType::PlainTriangle:
		return geometryTable[ PLAIN_TRIANGLE_GEOMETRY_IDX ];
	}

    return geometryTable[ PLAIN_TRIANGLE_GEOMETRY_IDX ];
}

std::unique_ptr< SamplerDataAttachment > AssetManager::getImage( const std::string& path )
{
	auto find = imageMap.find( path );

	if ( find == imageMap.end( ) )
	{
		loadImage( path );
		find = imageMap.find( path );
	}

    auto * copiedAttachment = new SamplerDataAttachment { };

    copiedAttachment->format = find->second->format;
    copiedAttachment->width = find->second->width;
    copiedAttachment->height = find->second->height;
    copiedAttachment->channels = find->second->channels;
    copiedAttachment->textureInfo = find->second->textureInfo;
    copiedAttachment->size = find->second->size;
    copiedAttachment->content = find->second->content;

	return std::move( std::unique_ptr< SamplerDataAttachment >( copiedAttachment ) );
}

int AssetManager::tryGetPrimitiveAttribute( const tinygltf::Primitive& primitive, const std::string& attribute )
{
	auto attributeSearch = primitive.attributes.find( attribute );

	if ( attributeSearch != primitive.attributes.end( ) )
	{
		return attributeSearch->second;
	}

	return -1;
}

glm::mat4 AssetManager::flatMatToGLMMat( const std::vector< float >& matFlat, int offset )
{
	return glm::make_mat4( matFlat.data( ) + offset );
}

void AssetManager::addNode( SceneContext& sceneContext, int parent, int nodeId )
{
	tinygltf::Node node = sceneContext.model.nodes[ nodeId ];

	MeshNode meshNode = { };
	meshNode.translation = glm::vec3( 0.0f );
	meshNode.scale = glm::vec3( 1.0f );
	meshNode.rotation = glm::quat( glm::mat4( 1.0f ) );

	glm::mat4 t = meshNode.getTransform( );

	if ( !node.translation.empty( ) )
	{
		meshNode.translation = glm::make_vec3( &node.translation[ 0 ] );
	}

	if ( !node.rotation.empty( ) )
	{
		meshNode.rotation = glm::make_quat( &node.rotation[ 0 ] );
		meshNode.rotation = glm::normalize( meshNode.rotation );
	}

	if ( !node.scale.empty( ) )
	{
		meshNode.scale = glm::make_vec3( &node.scale[ 0 ] );
	}

	if ( !node.matrix.empty( ) )
	{
		glm::mat4 transform = glm::make_mat4( node.matrix.data( ) );

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose( transform, meshNode.scale, meshNode.rotation, meshNode.translation, skew, perspective );
	}

	if ( parent == -1 )
	{
		sceneContext.nodeTree->addNode( nodeId, meshNode );
	}
	else
	{
		sceneContext.nodeTree->addNode( sceneContext.nodeTree->findNode( parent ), nodeId, meshNode );
	}
}

void AssetManager::attachMaterialData( SceneContext& context, ECS::IGameEntity* entity, int mesh )
{
	tinygltf::Mesh meshNode = context.model.meshes[ mesh ];

	if ( meshNode.primitives.empty( ) )
	{
		return;
	}

	tinygltf::Material material = context.model.materials[ meshNode.primitives[ 0 ].material ];

	auto baseColorFactor = material.values.find( "baseColorTexture" );

	if ( baseColorFactor == material.values.end( ) )
	{
		return;
	}

	auto elements = baseColorFactor->second.json_double_value;

	auto image = elements.find( "index" );

	if ( image == elements.end( ) )
	{
		return;
	}

	tinygltf::Image img = context.model.images[ ( int ) image->second ];
	entity->getComponent< ECS::CMaterial >( )->textures.push_back( ECS::Material::TextureInfo{ context.gltfModelDirectory + img.uri } );
}

AssetManager::~AssetManager( )
{
	for ( auto& imagePairs : imageMap )
	{
		stbi_image_free( imagePairs.second->content );
	}
}

END_NAMESPACES