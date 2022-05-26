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

#include "Common.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

class Utilities
{
private:
    Utilities( ) = default;

public:
    static glm::vec3 quatToEulerGlm( const btQuaternion &quat3 );

    static glm::vec3 toGlm( const btVector3 &vec3 );

    static glm::vec4 toGlm( const btVector4 &vec4 );

    static btVector3 toBt( glm::vec3 vec );

    static btQuaternion toBtQuat( glm::vec3 euler, bool isInRadians );

    static std::string readFile( const std::string &filename );

    static glm::mat4 getTRSMatrix( const glm::vec3 &t, const glm::quat &r, const glm::vec3 &s );

    static glm::quat vecToQuat( const glm::vec4 &vec );

    static std::string getFileDirectory( const std::string &file, bool includeFinalSep = true );

    static std::string getFilename( const std::string &file );

    static std::string combineDirectories( const std::string &directory, const std::string &file );

    static bool doesFileExist( const std::string &file );
};

END_NAMESPACES
