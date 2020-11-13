//
// Created by Murat on 11/13/2020.
//

#include "GLSLShader.h"

NAMESPACES( SomeVulkan, Graphics )

void GLSLShader::readFile( const std::string &filename ) {
    std::ifstream file( filename, std::ios::ate | std::ios::binary );

    if ( !file.is_open( ) ) {
        throw std::runtime_error( "failed to open file!" );
    }

    size_t fileSize = static_cast<size_t>( file.tellg( ) );
    contents.resize( fileSize );

    file.seekg( 0 );
    file.read( contents.data( ), fileSize );

    file.close( );
}

END_NAMESPACES