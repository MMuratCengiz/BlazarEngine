#pragma once

#include "../core/Common.h"
#include "../ecs/IGameEntity.h"
#include "../ecs/Renderable.h"
#include "../graphics/DrawDescription.h"

#include <string>
#include <strstream>
#include <utility>
#include <assimp/include/assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

NAMESPACES( SomeVulkan, RenderObjects )

class Mesh;

class Model {
private:
    std::vector< std::shared_ptr< ECS::IGameEntity > > modelEntities;
public:
    explicit Model( const std::string &path ) {
        Assimp::Importer import;

        const aiScene *scene = import.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs );

        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
            std::strstream ss;
            ss << "ERROR::ASSIMP::" << import.GetErrorString( );
            throw std::runtime_error( ss.str( ) );
        }

        onEachNode( scene, scene->mRootNode );
    }

    void onEachNode( const aiScene *scene, aiNode *pNode ) {
        for ( unsigned int i = 0; i < pNode->mNumChildren; ++i ) {
            onEachNode( scene, pNode->mChildren[ i ] );
        }

        for ( unsigned int m = 0; m < pNode->mNumMeshes; m++ ) {
            aiMesh *mesh = scene->mMeshes[ pNode->mMeshes[ m ] ];
            onEachMesh( scene, mesh );
        }
    }

    void onEachMesh( const aiScene *scene, aiMesh *mesh ) {
        auto pMesh = std::make_shared< Mesh >( );

        Graphics::DrawDescription drawDescription { };

        for ( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
            auto vec = mesh->mVertices[ i ];

            drawDescription.vertexMemory.attachElements( { vec.x, vec.y, vec.z } );

            if ( mesh->mTextureCoords[ 0 ] ) {
                aiVector3D &tCoor = mesh->mTextureCoords[ 0 ][ i ];

                drawDescription.vertexMemory.attachElements( { tCoor.x, tCoor.y } );
            }
        }

        for ( unsigned int f = 0; f < mesh->mNumFaces; f++ ) {
            aiFace face = mesh->mFaces[ f ];

            for ( unsigned int j = 0; j < face.mNumIndices; j++ ) {
                drawDescription.indices.emplace_back( face.mIndices[ j ] );
            }
        }

        modelEntities.emplace_back( pMesh );
    }

    std::vector< std::shared_ptr< ECS::IGameEntity > > getEntities() {
        return modelEntities;
    }
};


class Mesh : public ECS::IGameEntity {
    friend class Model;

    START_COMPONENTS
        RENDERABLE
    END_COMPONENTS
};

END_NAMESPACES