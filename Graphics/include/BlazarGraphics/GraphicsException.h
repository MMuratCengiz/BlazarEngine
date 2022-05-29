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

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class GraphicsException : public std::exception
{
private:
    std::string error;
public:
    GraphicsException( const std::string& source, const std::string &message )
    {
        std::stringstream formatter;
        formatter << "[" << source << "]" << ": " << message;
        error = formatter.str();
    }

    [[nodiscard]] const char *what( ) const noexcept override
    {
        return error.data( );
    }
};

END_NAMESPACES
