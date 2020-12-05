#pragma once

#include "../Core/Common.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct IComponent {
private:
    struct T_UID_COUNTER {
        uint64_t COUNT = 0;
    };

public:
    uint64_t uid;

	IComponent( ) {
        static T_UID_COUNTER UID_COUNTER{ };
        uid = UID_COUNTER.COUNT;
        UID_COUNTER.COUNT++;
	};

	virtual ~IComponent( ) { }
};

typedef std::shared_ptr< IComponent > pComponent;

END_NAMESPACES