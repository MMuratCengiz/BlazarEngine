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

#include "ResourceBinder.h"
#include "AreaTex.h"
#include "SearchTex.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

#define START_RESOURCE_DEFINITION( ) registerResourceBinder(
#define RESOURCE_NAME( name ) name,
#define RESOURCE_BIND_TYPE( bindType ) bindType,
#define RESOURCE_TYPE( resourceType ) resourceType,
#define FUNC( func ) ResourceBinderFunc( func )
#define END_RESOURCE_DEFINITION( ) );

#define ATTACH_EMPTY( )    UniformAttachmentContent result{ };             \
                           result.size = 0;                                \
                           return result;

#define ATTACH_DATA( data, type ) UniformAttachmentContent result{ };       \
                            result.size = sizeof( type );                   \
                            result.data = ( char * ) malloc( result.size ); \
                            result.resourceType = ResourceType::Uniform;    \
                            memcpy( result.data, &data, result.size );      \
                            return result;

ResourceBinder::ResourceBinder( )
{
    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "InstanceData" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> UniformAttachmentContent
                  {
                      auto data = DataAttachmentFormatter::formatInstances( entity->getComponent< ECS::CInstances >( ), entity );
                      ATTACH_DATA( data, InstanceData )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "OutlineColor" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> UniformAttachmentContent
            {
                auto outlineComponent = entity->getComponent< ECS::COutlined >( );

                if ( outlineComponent == nullptr )
                {
                    ATTACH_EMPTY( )
                }

                auto data = outlineComponent->outlineColor;
                ATTACH_DATA( data, glm::vec4 )
            } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "OutlineScale" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> UniformAttachmentContent
                  {
                      auto outlineComponent = entity->getComponent< ECS::COutlined >( );

                      if ( outlineComponent == nullptr )
                      {
                          ATTACH_EMPTY( )
                      }

                      auto data = glm::vec4( outlineComponent->borderScale );
                      ATTACH_DATA( data, glm::vec4 )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "WorldContext" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> UniformAttachmentContent
                  {
                      auto cameras = table->getComponents< ECS::CCamera >( );

                      auto activeCamera = cameras[ 0 ];

                      int i = 0;
                      while ( !activeCamera->isActive )
                      {
                          ASSERT_M( i < cameras.size( ), "You must   have a single active camera." );
                          activeCamera = cameras[ ++i ];
                      }

                      struct WorldContext
                      {
                          glm::vec4 cameraPosition;
                      };

                      WorldContext data{ };
                      data.cameraPosition = glm::vec4( activeCamera->position, 1.0f );
                      ATTACH_DATA( data, WorldContext )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "EnvironmentLights" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> UniformAttachmentContent
                  {
                      auto data = DataAttachmentFormatter::formatLightingEnvironment( table );
                      ATTACH_DATA( data, EnvironmentLights )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "LightViewProjectionMatrix" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &components ) -> UniformAttachmentContent
                  {
                      UniformAttachmentContent content { };

                      const auto directionalLights = components->getComponents< ECS::CDirectionalLight >( );
                      const auto cameras = components->getComponents< ECS::CCamera >( );

                      std::shared_ptr< ECS::CCamera > activeCamera;

                      for ( const auto &camera : cameras )
                      {
                          if ( camera->isActive )
                          {
                              activeCamera = camera;
                              break;
                          }
                      }

                      size_t mat4Size = 4 * 4 * sizeof( float );

                      uint32_t arraySize = 3;

                      content.size = 3 * mat4Size + sizeof( int );
                      content.data = ( char * ) malloc( content.size );

                      uint32_t offset = 0;

                      for ( const auto &light: directionalLights )
                      {
                          glm::mat4 lightProjection = glm::ortho( -100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 50.0f );

                          lightProjection = VK_CORRECTION_MATRIX * lightProjection;

                          glm::vec3 pos = glm::normalize( -light->direction ) * 30.0f;
                          glm::vec3 front = glm::vec3( 0, 0, 0 );

                          glm::mat4 lightView = glm::lookAt( pos, front, glm::vec3( 0.0f, 1.0f, 0.0f ) );

                          glm::mat4 result = lightProjection * lightView;

                          memcpy( content.data + offset, &result, mat4Size );
                          offset += mat4Size;

                          if ( offset == content.size )
                          {
                              break;
                          }
                      }

                      memcpy( content.data + ( content.size - sizeof( int ) ), &arraySize, sizeof( int ) );
                      return content;
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "ViewProjection" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> UniformAttachmentContent
                  {
                      auto data = DataAttachmentFormatter::formatCamera( table );
                      ATTACH_DATA( data, ViewProjection )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "Tessellation" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> UniformAttachmentContent
                  {
                      const std::shared_ptr< ECS::CTessellation > tessellation = entity->getComponent< ECS::CTessellation >( );

                      if ( tessellation == nullptr )
                      {
                          ATTACH_EMPTY( )
                      }

                      auto data = DataAttachmentFormatter::formatTessellationComponent( tessellation );
                      ATTACH_DATA( data, Tessellation )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "Material" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> UniformAttachmentContent
                  {
                      const std::shared_ptr< ECS::CMaterial > material = entity->getComponent< ECS::CMaterial >( );

                      if ( material == nullptr )
                      {
                          ATTACH_EMPTY( )
                      }

                      auto data = DataAttachmentFormatter::formatMaterialComponent( entity->getComponent< ECS::CMaterial >( ), entity->getComponent< ECS::CTransform >( ) );
                      ATTACH_DATA( data, Material )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "Resolution" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameUniform )
            RESOURCE_TYPE( ResourceType::Uniform )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> UniformAttachmentContent
                  {
                      auto gameStateComponent = table->getComponents< ECS::CGameState >( )[ 0 ];
                      auto data = DataAttachmentFormatter::formatResolution( gameStateComponent->surfaceWidth, gameStateComponent->surfaceHeight );
                      ATTACH_DATA( data, Resolution )
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "SkyBox" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameTexture )
            RESOURCE_TYPE( ResourceType::CubeMap )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> TextureAttachmentContent
                  {
                      std::vector< ECS::Material::TextureInfo > result;

                      for ( auto cubeMap : table->getComponents< ECS::CCubeMap >( ) )
                      {
                          for ( auto texture: cubeMap->texturePaths )
                          {
                              result.push_back( ECS::Material::TextureInfo { texture.path } );
                          }
                      }

                      TextureAttachmentContent attachment { };
                      attachment.resourceType = ResourceType::CubeMap;
                      attachment.textures = result;

                      return attachment;
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "searchTex" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameTexture )
            RESOURCE_TYPE( ResourceType::Sampler2D )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> TextureAttachmentContent
                  {
                      std::vector< ECS::Material::TextureInfo > result;

                      auto &searchTextInfo = result.emplace_back( ECS::Material::TextureInfo { } );

                      searchTextInfo.U = ECS::Material::AddressMode::ClampToEdge;
                      searchTextInfo.V = ECS::Material::AddressMode::ClampToEdge;
                      searchTextInfo.W = ECS::Material::AddressMode::ClampToEdge;

                      searchTextInfo.isInMemory = true;
                      searchTextInfo.inMemoryTexture = { };
                      searchTextInfo.inMemoryTexture.contents = searchTexBytes;
                      searchTextInfo.inMemoryTexture.width = SEARCHTEX_WIDTH;
                      searchTextInfo.inMemoryTexture.height = SEARCHTEX_HEIGHT;
                      searchTextInfo.inMemoryTexture.channels = 4;
                      searchTextInfo.inMemoryTexture.format = ECS::Material::ImageFormat::R8Unorm;

                      TextureAttachmentContent attachment { };
                      attachment.resourceType = ResourceType::Sampler2D;
                      attachment.textures = result;

                      return attachment;
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "areaTex" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerFrameTexture )
            RESOURCE_TYPE( ResourceType::Sampler2D )
            FUNC( [ ]( const std::shared_ptr< ECS::ComponentTable > &table ) -> TextureAttachmentContent
                  {
                      std::vector< ECS::Material::TextureInfo > result;

                      auto &areaTexInfo = result.emplace_back( ECS::Material::TextureInfo { } );

                      areaTexInfo.U = ECS::Material::AddressMode::ClampToEdge;
                      areaTexInfo.V = ECS::Material::AddressMode::ClampToEdge;
                      areaTexInfo.W = ECS::Material::AddressMode::ClampToEdge;

                      areaTexInfo.isInMemory = true;
                      areaTexInfo.inMemoryTexture = { };
                      areaTexInfo.inMemoryTexture.contents = areaTexBytes;
                      areaTexInfo.inMemoryTexture.width = AREATEX_WIDTH;
                      areaTexInfo.inMemoryTexture.height = AREATEX_HEIGHT;
                      areaTexInfo.inMemoryTexture.channels = 4;
                      areaTexInfo.inMemoryTexture.format = ECS::Material::ImageFormat::R8G8Unorm;

                      TextureAttachmentContent attachment { };
                      attachment.resourceType = ResourceType::Sampler2D;
                      attachment.textures = result;

                      return attachment;
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "HeightMap" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityTexture )
            RESOURCE_TYPE( ResourceType::Sampler2D )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> TextureAttachmentContent
                  {
                      std::vector< ECS::Material::TextureInfo > result;

                      const std::shared_ptr< ECS::CMaterial > material = entity->getComponent< ECS::CMaterial >( );

                      if ( material != nullptr && !material->heightMap.path.empty( ) )
                      {
                          result.push_back( material->heightMap );
                      }

                      TextureAttachmentContent attachment { };
                      attachment.resourceType = ResourceType::Sampler2D;
                      attachment.textures = result;

                      return attachment;
                  } )
    END_RESOURCE_DEFINITION( )

    START_RESOURCE_DEFINITION( )
            RESOURCE_NAME( "Texture1" )
            RESOURCE_BIND_TYPE( ResourceBindType::PerEntityTexture )
            RESOURCE_TYPE( ResourceType::Sampler2D )
            FUNC( [ ]( const std::shared_ptr< ECS::IGameEntity > &entity ) -> TextureAttachmentContent
                  {
                      std::vector< ECS::Material::TextureInfo > result;

                      const std::shared_ptr< ECS::CMaterial > material = entity->getComponent< ECS::CMaterial >( );

                      if ( material != nullptr && !material->textures[ 0 ].path.empty( ) )
                      {
                          result.push_back( material->textures[ 0 ] );
                      }

                      TextureAttachmentContent attachment { };
                      attachment.resourceType = ResourceType::Sampler2D;
                      attachment.textures = result;

                      return attachment;
                  } )
    END_RESOURCE_DEFINITION( )
}

void ResourceBinder::registerResourceBinder( const std::string &resourceName, const ResourceBindType &bindType, const ResourceType &resourceType, ResourceBinderFunc binder )
{
    resourceBindTypes[ resourceName ] = bindType;
    resourceTypes[ resourceName ] = resourceType;

    if ( bindType == ResourceBindType::PerFrameUniform || bindType == ResourceBindType::PerFrameTexture )
    {
        resourceBindStrategies[ resourceName ] = ResourceBindStrategy::BindPerFrame;
    }
    else if ( bindType == ResourceBindType::PerEntityUniform || bindType == ResourceBindType::PerEntityTexture )
    {
        resourceBindStrategies[ resourceName ] = ResourceBindStrategy::BindPerObject;
    }

    switch ( bindType )
    {
        case ResourceBindType::PerFrameUniform:
            perFrameUniformBinders[ resourceName ] = std::move( binder.perFrameUniformBinder );
            break;
        case ResourceBindType::PerEntityUniform:
            perEntityUniformBinders[ resourceName ] = std::move( binder.perEntityUniformBinder );
            break;
        case ResourceBindType::PerFrameTexture:
            perFrameTextureBinders[ resourceName ] = std::move( binder.perFrameTextureBinder );
            break;
        case ResourceBindType::PerEntityTexture:
            perEntityTextureBinders[ resourceName ] = std::move( binder.perEntityTextureBinder );
            break;
    }
}

ResourceBindStrategy ResourceBinder::getResourceBindStrategy( const std::string &resourceName ) const
{
    return resourceBindStrategies.at( resourceName );
}

ResourceType ResourceBinder::getResourceType( const std::string &resourceName ) const
{
    return resourceTypes.at( resourceName );
}

ResourceBindType ResourceBinder::getResourceBindType( const std::string &resourceName ) const
{
    return resourceBindTypes.at( resourceName );
}

std::optional< ResourceBindType > ResourceBinder::getResourceBindTypeOptional( const std::string &resourceName ) const
{
    auto find = resourceBindTypes.find( resourceName );

    if ( find == resourceBindTypes.end( ) )
    {
        return std::optional< ResourceBindType >( );
    }

    return std::optional< ResourceBindType >( find->second );
}


PerFrameUniformBinder ResourceBinder::getResourcePerFrameUniformBinder( const std::string &resourceName ) const
{
    return perFrameUniformBinders.at( resourceName );
}

PerEntityUniformBinder ResourceBinder::getResourcePerEntityUniformBinder( const std::string &resourceName ) const
{
    return perEntityUniformBinders.at( resourceName );
}

PerFrameTextureBinder ResourceBinder::getResourcePerFrameTextureBinder( const std::string &resourceName ) const
{
    return perFrameTextureBinders.at( resourceName );
}

PerEntityTextureBinder ResourceBinder::getResourcePerEntityTextureBinder( const std::string &resourceName ) const
{
    return perEntityTextureBinders.at( resourceName );
}

std::vector< std::string > ResourceBinder::getAllPerEntityBinders( ) const
{
    std::vector< std::string > result;

    for ( const auto &pair: perEntityTextureBinders )
    {
        result.push_back( pair.first );
    }

    for ( const auto &pair: perEntityUniformBinders )
    {
        result.push_back( pair.first );
    }

    return result;
}

#undef START_RESOURCE_DEFINITION
#undef RESOURCE_NAME
#undef RESOURCE_BIND_TYPE
#undef FUNC
#undef END_RESOURCE_DEFINITION

END_NAMESPACES
