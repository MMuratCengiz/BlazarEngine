//
// Created by Murat on 11/13/2020.
//

#include "GLSLShader.h"

#include <spirv_cross/spirv_cross.hpp>

NAMESPACES( SomeVulkan, Graphics )

GLSLShader::GLSLShader( ShaderType2 type, const std::string& path ) {
	auto contents = readFile( path );

	spirv_cross::Compiler compiler( move( contents ) );

	auto shaderResources = compiler.get_shader_resources( );

	auto stageInputs = shaderResources.stage_inputs;

	vertexAttributeDescriptions.resize( stageInputs.size( ) );
	
	for ( const auto& resource : stageInputs ) {
		
	}
}

std::vector< uint32_t > GLSLShader::readFile( const std::string& filename ) {
	FILE* file = fopen( filename.c_str( ), "rb" );
	if ( !file ) {
		throw std::runtime_error( "Failed to load shader: " + filename + "." );
	}

	fseek( file, 0, SEEK_END );
	long fileSize = ftell( file ) / sizeof( uint32_t );
	rewind( file );

	std::vector< uint32_t > contents( fileSize );

	if ( fread( contents.data( ), sizeof( uint32_t ), fileSize, file ) != size_t( fileSize ) ) {
		contents.clear( );
	}

	fclose( file );
	return contents;
}

END_NAMESPACES