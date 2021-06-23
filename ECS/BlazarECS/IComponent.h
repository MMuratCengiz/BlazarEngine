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

struct IComponent
{
public:
    const std::type_index typeId;
    uint64_t uid;

    inline explicit IComponent( const std::type_index& typeId ) : typeId( typeId )
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

#define BLAZAR_COMPONENT( ClassType ) ClassType( ) : IComponent( typeid( ClassType ) ) { } ~ClassType( ) override = default;

#define BLAZAR_COMPONENT_CUSTOM_DESTRUCTOR( ClassType ) ClassType( ) : IComponent( typeid( ClassType ) ) { }

END_NAMESPACES