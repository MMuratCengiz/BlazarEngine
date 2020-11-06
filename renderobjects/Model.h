#pragma once

#include "../core/Common.h"
#include "../ecs/IGameEntity.h"
#include "../ecs/Renderable.h"
#include "../graphics/DrawDescription.h"
#include "../graphics/Texture.h"

#include <string>
#include <sstream>
#include <utility>
#include <assimp/include/assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

NAMESPACES( SomeVulkan, RenderObjects )

class Model;

class Mesh : public ECS::IGameEntity {
    Graphics::DrawDescription drawDescription{ };
    std::shared_ptr< Graphics::Texture > texture = std::make_shared< Graphics::Texture >( 2,
                                                                                          "/assets/textures/viking_room.png" );

    friend class Model;

    START_COMPONENTS
        RENDERABLE
    END_COMPONENTS
};

class Model {
private:
    std::vector< std::shared_ptr< ECS::IGameEntity > > modelEntities;
public:
    explicit Model( const std::string &path ) {
        Assimp::Importer import;

        const aiScene *scene = import.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs );

        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
            std::stringstream ss;
            ss << "ERROR::ASSIMP::" << import.GetErrorString( );
            throw std::runtime_error( ss.str( ) );
        }

        onEachNode( scene, scene->mRootNode );
    }

    void onEachNode( const aiScene *scene, const aiNode *pNode ) {
        for ( unsigned int i = 0; i < pNode->mNumChildren; ++i ) {
            onEachNode( scene, pNode->mChildren[ i ] );
        }

        for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ ) {
            aiMesh *mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];
            onEachMesh( mesh );
        }
    }

    void onEachMesh( const aiMesh *mesh ) {
        auto pMesh = std::make_shared< Mesh >( );

        Graphics::DrawDescription& drawDescription = pMesh->drawDescription;

        drawDescription.vertexCount = mesh->mNumVertices;
        drawDescription.indexedMode = true;
        drawDescription.vertexMemory.setInitialSize( mesh->mNumVertices * 5 * sizeof( float ) );
        drawDescription.textures.emplace_back( pMesh->texture );

        for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
            auto vec = mesh->mVertices[ i ];

            drawDescription.vertexMemory.attachElements< float >( { vec.x, vec.y, vec.z } );

            if ( mesh->mTextureCoords[ 0 ] ) {
                aiVector3D &tCoor = mesh->mTextureCoords[ 0 ][ i ];

                drawDescription.vertexMemory.attachElements< float >( { tCoor.x , tCoor.y } );
            }
        }

        for ( unsigned int f = 0; f < mesh->mNumFaces; f++ ) {
            aiFace face = mesh->mFaces[ f ];

            for ( unsigned int j = 0; j < face.mNumIndices; j++ ) {
                drawDescription.indices.emplace_back( face.mIndices[ j ] );
            }
        }

        pMesh->getComponent< ECS::Renderable >()->setDrawDescription( drawDescription );
        modelEntities.emplace_back( pMesh );
    }

    std::vector< std::shared_ptr< ECS::IGameEntity > > getEntities() {
        return modelEntities;
    }
};


END_NAMESPACES