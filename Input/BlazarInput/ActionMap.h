#pragma once

#include <BlazarCore/Common.h>
#include "EventHandler.h"

NAMESPACES( ENGINE_NAMESPACE, Input )

typedef std::function< void( std::string ) > ActionCallback;

enum class Controller
{
    Keyboard,
    Mouse,
};

struct ActionBinding
{
    Controller controller;
    KeyPressForm pressForm;

    // If controller == Keyboard
    KeyboardKeyCode keyCode;
};

class ActionMap
{
private:
    std::shared_ptr< EventHandler > eventHandler;
    std::unordered_map< std::string, std::vector< ActionCallback > > callbacks;
    ActionCallback proxyActionCallback;
public:
    explicit ActionMap( std::shared_ptr< EventHandler > eventHandler );

    void registerAction( const std::string &actionName, ActionBinding binding );
    void subscribeToAction( const std::string &actionName, ActionCallback callback );
};

END_NAMESPACES
