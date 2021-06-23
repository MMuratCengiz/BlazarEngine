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
    EventHandler* eventHandler;
    std::unordered_map< std::string, std::vector< ActionCallback > > callbacks;
    ActionCallback proxyActionCallback;
public:
    explicit ActionMap( EventHandler* eventHandler );

    void registerAction( const std::string &actionName, ActionBinding binding );
    void subscribeToAction( const std::string &actionName, ActionCallback callback );
};

END_NAMESPACES
