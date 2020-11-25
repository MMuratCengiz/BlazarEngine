#pragma once

#include "../Core/Common.h"

NAMESPACES( SomeVulkan, Input )

struct IEventParameters {
    void * userPointer{}; // todo maybe remove
    virtual ~IEventParameters() = default;
};

struct WindowResizedParameters : IEventParameters {
    uint32_t width;
    uint32_t height;
};

enum class EventType {
    WindowResized,
    SwapChainInvalidated,
};

namespace FunctionDefinitions {
typedef std::function< void( const EventType& type, std::shared_ptr< IEventParameters > eventParams ) > EventCallback;
}


class GlobalEventHandler {
private:
    GlobalEventHandler( ) = default;
    std::unordered_map< EventType, std::vector< FunctionDefinitions::EventCallback > > eventSubscribers;
public:
    void initWindowEvents( GLFWwindow* window );
    void subscribeToEvent( const EventType& event, const FunctionDefinitions::EventCallback& cb );
    void triggerEvent( const EventType& event, const std::shared_ptr< IEventParameters >& parameters );

    static GlobalEventHandler &Instance( ) {
        static GlobalEventHandler inst { };
        return inst;
    }

    inline static std::shared_ptr< WindowResizedParameters > ToWindowResizedParameters( const std::shared_ptr< IEventParameters >& parameters ) {
        return std::dynamic_pointer_cast< WindowResizedParameters >( parameters );
    }

    GlobalEventHandler( GlobalEventHandler const & ) = delete;
    void operator=( GlobalEventHandler const & ) = delete;
private:
    void ensureMapContainsEvent( const EventType& event );
};

typedef std::shared_ptr< IEventParameters > pEventParameters;

END_NAMESPACES