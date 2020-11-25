#pragma once

#include "../Core/Common.h"
#include "EventHandler.h"

NAMESPACES( SomeVulkan, Input )

typedef std::function< void( std::string ) > ActionCallback;

enum class Controller {
    Keyboard,
    Mouse,
};

struct ActionBinding {
    Controller controller;
    KeyPressForm pressForm;

    // If controller == Keyboard
    KeyboardKeyCode keyCode;
};

class ActionMap {
private:
    std::shared_ptr< EventHandler > eventHandler;
    std::unordered_map< std::string, std::vector< ActionCallback > > callbacks;
public:
    explicit ActionMap( std::shared_ptr< EventHandler > eventHandler );
    void registerAction( const std::string& actionName, const ActionBinding& binding, const ActionCallback& callback );
};

END_NAMESPACES
