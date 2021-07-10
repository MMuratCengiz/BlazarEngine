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
#include <mutex>
#include <typeindex>

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct ComponentTypeRef
{
private:
    uint64_t typeCount = 0;

    template< class T >
    struct IdCounter
    {
        inline static uint64_t instanceCount = 0;
        inline static uint64_t assignedType = 0;

        IdCounter( )
        {
            instanceCount++;
        }
    };
public:
    static ComponentTypeRef& get()
    {
        static ComponentTypeRef instance;
        return instance;
    }

    template< class T >
    uint64_t getTypeId( )
    {
        IdCounter< T > countDummy{ };

        if ( countDummy.instanceCount == 1 )
        {
            IdCounter< T >::assignedType = typeCount;
            return typeCount++;
        }

        return IdCounter< T >::assignedType;
    }
};

struct IComponent
{
public:
    const uint64_t typeId;
    uint64_t uid;

    inline explicit IComponent( const uint64_t& typeId ) : typeId( typeId )
    {
        static std::mutex uidGenLock;
        static uint64_t entityUidCounter = 0;

        uidGenLock.lock( );
        uid = entityUidCounter++;
        uidGenLock.unlock( );
    };

    virtual ~IComponent( ) = default;
};

typedef std::shared_ptr< IComponent > pComponent;

#define BLAZAR_UNIQUE_TYPE_ID( ClassType ) ComponentTypeRef::get().getTypeId< ClassType >( )

#define BLAZAR_COMPONENT( ClassType ) ClassType( ) : IComponent( BLAZAR_UNIQUE_TYPE_ID( ClassType ) ) { } ~ClassType( ) override = default;

#define BLAZAR_COMPONENT_CUSTOM_DESTRUCTOR( ClassType ) ClassType( ) : IComponent( BLAZAR_UNIQUE_TYPE_ID( ClassType ) ) { }

END_NAMESPACES