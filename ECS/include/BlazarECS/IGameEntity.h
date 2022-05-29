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

#include <typeindex>
#include <typeinfo>
#include <BlazarCore/Common.h>
#include "IComponent.h"
#include "CTransform.h"
#include <mutex>
#include <vector>
#include <unordered_map>

NAMESPACES( ENGINE_NAMESPACE, ECS )

class IGameEntity
{
private:
	std::vector< std::unique_ptr< IComponent > > componentQuickAccess;
	std::vector< uint64_t > componentList;
	std::vector< IGameEntity * > children;
    std::vector< std::unique_ptr< IGameEntity > > managedChildren;
	uint64_t uid;
public:
	IGameEntity( )
	{
		static std::mutex uidGenLock;
		static uint64_t entityUidCounter = 0;

		uidGenLock.lock( );
		uid = entityUidCounter++;
		uidGenLock.unlock( );

		createComponent< CTransform >( );
	};

	inline void addChild( IGameEntity * child )
	{
		children.push_back( child );
	}

	inline void addManagedChild( std::unique_ptr< IGameEntity > child )
	{
        auto & c = managedChildren.emplace_back( std::move( child ) );
		children.push_back( c.get( ) );
	}

	[[nodiscard]] const uint64_t& getUID( ) const noexcept
	{
		return uid;
	}

    [[nodiscard]] const std::vector< IGameEntity * >& getChildren( ) const noexcept
	{
		return children;
	}

	template < class T >
	bool hasComponent( ) noexcept
	{
		return getComponent< T >( ) != nullptr;
	}

	template < class CastAs >
	CastAs * getComponent( ) noexcept
	{
        const uint64_t typeId = ComponentTypeRef::get( ).getTypeId< CastAs >( );

		if ( typeId >= componentQuickAccess.size( ) )
		{
			return nullptr;
		}

        const auto& component = componentQuickAccess[ typeId ];

		if ( component == nullptr )
		{
			return nullptr;
		}

		return ( CastAs * )( component.get() );
	}

    [[nodiscard]] std::vector< IComponent * > getAllComponents( ) const noexcept
	{
		std::vector< IComponent * > result{ componentList.size( ) };

		for ( int i = 0; i < componentList.size(  ); ++i )
		{
			result[ i ] = componentQuickAccess[ componentList[ i ] ].get();
		}

		return result;
	}

	template < class CType >
	CType * createComponent( )
	{
		std::unique_ptr< CType > newComponent = std::make_unique< CType >( );

        const uint64_t typeId = newComponent->typeId;

		if ( typeId >= componentQuickAccess.size( ) )
		{
			componentQuickAccess.resize( typeId + 1 );
		}
		else if ( componentQuickAccess[ typeId ] != nullptr )
		{
			return ( CType * )( componentQuickAccess[ typeId ].get() );
		}

		componentList.push_back( typeId );
		componentQuickAccess[ typeId ] = std::move( newComponent );
		return getComponent< CType >( );
	}

	virtual ~IGameEntity( ) = default;
};

class DynamicGameEntity : public IGameEntity
{
public:
	~DynamicGameEntity( ) override = default;
};

END_NAMESPACES
