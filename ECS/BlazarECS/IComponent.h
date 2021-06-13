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