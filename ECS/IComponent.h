#pragma once

#include "../Core/Common.h"
#include <mutex>

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct IComponent
{
public:
    uint64_t uid;

    IComponent( )
    {
        static std::mutex uidGenLock;
        static uint64_t entityUidCounter = 0;

        uidGenLock.lock( );
        uid = entityUidCounter++;
        uidGenLock.unlock( );
    };

    virtual ~IComponent( )
    { }
};

typedef std::shared_ptr< IComponent > pComponent;

END_NAMESPACES