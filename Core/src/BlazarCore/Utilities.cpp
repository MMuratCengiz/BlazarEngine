// Blazar Engine - 3D Game Engine
// Copyright (c) 2020-2021 Muhammed Murat Cengiz
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <fstream>
#include <BlazarCore/Utilities.h>
#include <vector>
#include <glm/gtc/quaternion.hpp>

NAMESPACES( ENGINE_NAMESPACE, Core )

glm::vec3 Utilities::quatToEulerGlm( const btQuaternion &quat3 )
{
    glm::vec3 result;

    quat3.getEulerZYX( result.z, result.y, result.x );

    return result;
}

glm::vec3 Utilities::toGlm( const btVector3 &vec3 )
{
    return glm::vec3(
            vec3.getX( ),
            vec3.getY( ),
            vec3.getZ( )
    );
}

glm::vec4 Utilities::toGlm( const btVector4 &vec4 )
{
    return glm::vec4(
            vec4.getX( ),
            vec4.getY( ),
            vec4.getZ( ),
            vec4.getW( )
    );
}

btVector3 Utilities::toBt( glm::vec3 vec )
{
    return btVector3
            {
                    vec.x,
                    vec.y,
                    vec.z
            };
}

btQuaternion Utilities::toBtQuat( glm::vec3 euler, bool isInRadians )
{
    btQuaternion quaternion { };

    if ( isInRadians )
    {
        quaternion.setEuler(
                euler.x,
                euler.y,
                euler.z
        );
    }
    else
    {
        quaternion.setEuler(
                glm::radians( euler.x ),
                glm::radians( euler.y ),
                glm::radians( euler.z )
        );
    }

    return btQuaternion( );
}


std::string Utilities::readFile( const std::string &filename )
{
    std::ifstream file( filename, std::ios::ate | std::ios::binary );

    if ( !file.is_open( ) )
    {
        throw std::runtime_error( "failed to open file!" );
    }

    std::string data;

    file.seekg( 0, std::ios::end );
    data.reserve( file.tellg( ) );
    file.seekg( 0, std::ios::beg );
    data.assign( ( std::istreambuf_iterator< char >( file ) ), std::istreambuf_iterator< char >( ) );

    return std::move( data );

}

glm::mat4 Utilities::getTRSMatrix( const glm::vec3 &t, const glm::quat &r, const glm::vec3 &s )
{
    glm::mat4 modelMatrix { 1 };

    modelMatrix = glm::translate( modelMatrix, t );
    modelMatrix = glm::scale( modelMatrix, s );
    modelMatrix *= glm::mat4_cast( r );

    return modelMatrix;
}

glm::quat Utilities::vecToQuat( const glm::vec4 &vec )
{
    return glm::make_quat( glm::value_ptr( vec ) );
}

std::string Utilities::getFileDirectory( const std::string &file, bool includeFinalSep )
{
    size_t sepUnixIdx = file.find_last_of( "/\\" );
    size_t sepWinIdx = file.find_last_of( "\\\\" );

    int finalSepSub = includeFinalSep ? 1 : 0;

    if ( sepUnixIdx != -1 )
    {
        return file.substr( 0, sepUnixIdx + finalSepSub );
    }
    else if ( sepWinIdx != -1 )
    {
        return file.substr( 0, sepWinIdx - finalSepSub );
    }

    return file;
}

std::string Utilities::getFilename( const std::string &file )
{
    size_t sepUnixIdx = file.find_last_of( "/\\" );
    size_t sepWinIdx = file.find_last_of( "\\\\" );

    if ( sepUnixIdx != -1 )
    {
        return file.substr( sepUnixIdx + 1 );
    }
    else if ( sepWinIdx != -1 )
    {
        return file.substr( 0, sepWinIdx + 1 );
    }

    return file;
}

std::string Utilities::combineDirectories( const std::string &directory, const std::string &file )
{
    std::string dir = getFileDirectory( directory );
    std::string f = getFilename( file );

    return dir + f;

}

bool Utilities::doesFileExist( const std::string &file )
{
#ifdef WIN32
    return false;
    // return _access(file.c_str(), 0) != -1;
#else
    return false;
    //return access( file.c_str(), 0 ) != -1;
#endif
}

END_NAMESPACES

