#include "ComponentTable.h"

void BlazarEngine::ECS::ComponentTable::addAllEntityComponentRecursive( const std::shared_ptr< IGameEntity >& gameEntity )
{
    for ( const auto& component: gameEntity->getAllComponents( ) )
    {
        addNewComponent( component );
    }

    for ( auto child: gameEntity->getChildren( ) )
    {
        addAllEntityComponentRecursive( child );
    }
}

void BlazarEngine::ECS::ComponentTable::addNewComponent( std::shared_ptr< IComponent > component )
{
    auto componentList = componentTable.find( component->typeId );

    if ( componentList == componentTable.end( ) )
    {
        componentTable[ component->typeId ] = { };
        componentList = componentTable.find( component->typeId );
    }

    componentList->second.push_back( std::move( component ) );
}

void BlazarEngine::ECS::ComponentTable::removeComponent( const std::shared_ptr< IComponent > &component )
{
    auto componentList = componentTable.find( component->typeId );

    FUNCTION_BREAK( componentList == componentTable.end( ) ) // Nothing to do

    for ( auto it = componentList->second.begin( ); it != componentList->second.end( ); ++it )
    {
        if ( it->get( )->uid == component->uid )
        {
            componentList->second.erase( it );
        }
    }
}

