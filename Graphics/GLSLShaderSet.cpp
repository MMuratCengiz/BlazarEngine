//
// Created by Murat on 11/13/2020.
//

#include "GLSLShaderSet.h"

#include <spirv_cross/spirv_cross.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

GLSLShaderSet::GLSLShaderSet( const std::vector< ShaderInfo > &shaderInfos, const bool &interleavedMode ) : interleavedMode( interleavedMode )
{
    for ( const ShaderInfo &shaderInfo : shaderInfos )
    {
        onEachShader( shaderInfo );
    }
}

void GLSLShaderSet::onEachShader( const ShaderInfo &shaderInfo )
{
    auto contents = readFile( shaderInfo.path );

    spirv_cross::Compiler compiler( move( contents ) );

    auto shaderResources = compiler.get_shader_resources( );

    auto stageInputs = shaderResources.stage_inputs;
    auto samplers = shaderResources.sampled_images;
    auto uniforms = shaderResources.uniform_buffers;

    uint32_t offsetIter = 0;

    // TODO is this fine? if so maybe throw an error if no vertex shader i
    if ( shaderInfo.type == vk::ShaderStageFlagBits::eVertex )
    {
        std::sort( stageInputs.begin( ), stageInputs.end( ), [ & ]( const spirv_cross::Resource &r1, const spirv_cross::Resource &r2 )
        {
            SpvDecoration decoration1 = getDecoration( compiler, r1 );
            SpvDecoration decoration2 = getDecoration( compiler, r2 );
            return decoration1.location < decoration2.location;
        } );

        for ( const spirv_cross::Resource &resource : stageInputs )
        {
            SpvDecoration decoration = getDecoration( compiler, resource );
            GLSLShaderSet::GLSLType gType = spvToGLSLType( decoration.type );
            createVertexInput( offsetIter, gType, decoration.location );
            offsetIter += gType.size;
        }

        if ( interleavedMode )
        {
            vk::VertexInputBindingDescription &bindingDesc = inputBindingDescriptions.emplace_back( vk::VertexInputBindingDescription { } );
            bindingDesc.binding = 0;
            bindingDesc.inputRate = vk::VertexInputRate::eVertex; // TODO investigate later for instanced rendering
            bindingDesc.stride = offsetIter;
        }
    }

    for ( const spirv_cross::Resource &resource : samplers )
    {
        DescriptorBindingCreateInfo createInfo;
        createInfo.binding = 0;
        createInfo.resource = resource;
        createInfo.stage = shaderInfo.type;
        createInfo.type = vk::DescriptorType::eCombinedImageSampler;

        createDescriptorSetBinding( compiler, createInfo );
    }

    for ( const spirv_cross::Resource &resource : uniforms )
    {
        DescriptorBindingCreateInfo createInfo;
        createInfo.binding = 0;
        createInfo.resource = resource;
        createInfo.stage = shaderInfo.type;
        createInfo.type = vk::DescriptorType::eUniformBuffer;

        createDescriptorSetBinding( compiler, createInfo );
    }
}

std::vector< uint32_t > GLSLShaderSet::readFile( const std::string &filename )
{
    FILE *file = fopen( filename.c_str( ), "rb" );
    if ( !file )
    {
        throw std::runtime_error( "Failed to load shader: " + filename + "." );
    }

    fseek( file, 0, SEEK_END );
    long fileSize = ftell( file ) / sizeof( uint32_t );
    rewind( file );

    std::vector< uint32_t > contents( fileSize );

    if ( fread( contents.data( ), sizeof( uint32_t ), fileSize, file ) != size_t( fileSize ) )
    {
        contents.clear( );
    }

    fclose( file );
    return contents;
}

void GLSLShaderSet::ensureSetExists( uint32_t set )
{
    if ( descriptorSetMap.find( set ) == descriptorSetMap.end( ) )
    {
        descriptorSetMap[ set ] = DescriptorSet { };
        descriptorSetMap[ set ].id = set;
    }
}

void GLSLShaderSet::createVertexInput( const uint32_t &offset, const GLSLType &type, const uint32_t &location )
{
    vk::VertexInputAttributeDescription &desc = vertexAttributeDescriptions.emplace_back( vk::VertexInputAttributeDescription { } );

    if ( interleavedMode )
    {
        desc.binding = 0;
    } else
    {
        vk::VertexInputBindingDescription &bindingDesc = inputBindingDescriptions.emplace_back( vk::VertexInputBindingDescription { } );
        bindingDesc.binding = inputBindingDescriptions.size( ) - 1;
        bindingDesc.inputRate = vk::VertexInputRate::eVertex; // TODO investigate later for instanced rendering
        bindingDesc.stride = 0;

        desc.binding = bindingDesc.binding;
    }

    desc.location = location;
    desc.format = type.format;
    desc.offset = offset;
}


void GLSLShaderSet::createDescriptorSetBinding( const spirv_cross::Compiler &compiler, const DescriptorBindingCreateInfo &bindingCreateInfo )
{
    SpvDecoration decoration = getDecoration( compiler, bindingCreateInfo.resource );

    auto stages = compiler.get_entry_points_and_stages( );

    DescriptorSet &descriptorSet = descriptorSetMap[ decoration.set ];

    vk::DescriptorSetLayoutBinding &layoutBinding = descriptorSet.descriptorSetLayoutBindings.emplace_back( vk::DescriptorSetLayoutBinding { } );

    layoutBinding.binding = decoration.binding;
    layoutBinding.descriptorType = bindingCreateInfo.type;
    layoutBinding.descriptorCount = decoration.arraySize;
    layoutBinding.stageFlags = bindingCreateInfo.stage;

    DescriptorSetBinding &binding = descriptorSet.descriptorSetBindings.emplace_back( DescriptorSetBinding { } );
    binding.index = descriptorSet.descriptorSetBindings.size( ) - 1;
    binding.size = decoration.size;
    binding.type = bindingCreateInfo.type;
    binding.name = decoration.name;
    binding.binding = layoutBinding;

    descriptorSet.descriptorSetBindingMap[ decoration.name ] = binding;
    descriptorSets.emplace_back( descriptorSet );
}


GLSLShaderSet::GLSLType GLSLShaderSet::spvToGLSLType( const spirv_cross::SPIRType &type )
{

#define not_supported( )

#define makeFormat_1( bits, baseType ) vk::Format::eR##bits####baseType
#define makeFormat_2( bits, baseType ) vk::Format::eR##bits##G##bits####baseType
#define makeFormat_3( bits, baseType ) vk::Format::eR##bits##G##bits##B##bits####baseType
#define makeFormat_4( bits, baseType ) vk::Format::eR##bits##G##bits##B##bits##A##bits####baseType
#define mfc( count, bits, baseType ) if ( type.vecsize == count ) format = makeFormat_##count( bits, baseType );
#define makeFormat( bits, baseType ) mfc( 1, bits, baseType ) mfc( 2, bits, baseType ) mfc( 3, bits, baseType ) mfc( 4, bits, baseType )

    vk::Format format = vk::Format::eUndefined;
    uint32_t size = 0;

    switch ( type.basetype )
    {
        default:
            not_supported( );
            break;
        case spirv_cross::SPIRType::Void:
            break;
        case spirv_cross::SPIRType::Boolean:
            break;
        case spirv_cross::SPIRType::Short:
        case spirv_cross::SPIRType::UShort:
        case spirv_cross::SPIRType::Int:
        makeFormat( 32, Sint );
            size = sizeof( int32_t );
            break;
        case spirv_cross::SPIRType::Int64:
        makeFormat( 32, Sint );
            size = sizeof( int64_t );
            break;
        case spirv_cross::SPIRType::UInt:
        makeFormat( 32, Uint );
            size = sizeof( uint32_t );
            break;
        case spirv_cross::SPIRType::UInt64:
        makeFormat( 64, Uint );
            size = sizeof( uint64_t );
            break;
        case spirv_cross::SPIRType::Float:
        makeFormat( 32, Sfloat );
            size = sizeof( float );
            break;
        case spirv_cross::SPIRType::Double:
        makeFormat( 64, Sfloat );
            size = sizeof( double ); // todo test
            break;
        case spirv_cross::SPIRType::Struct:
            break;
        case spirv_cross::SPIRType::Image:
            break;
        case spirv_cross::SPIRType::SampledImage:
            break;
        case spirv_cross::SPIRType::Sampler:
            break;
        case spirv_cross::SPIRType::AccelerationStructure:
            break;
        case spirv_cross::SPIRType::RayQuery:
            break;
        case spirv_cross::SPIRType::ControlPointArray:
            break;
    }

#undef makeFormat_1
#undef makeFormat_2
#undef makeFormat_3
#undef makeFormat_4
#undef makeFormat
#undef mfc
#undef not_supported

    return GLSLType { format, size * type.vecsize };
}

GLSLShaderSet::SpvDecoration GLSLShaderSet::getDecoration( const spirv_cross::Compiler &compiler, const spirv_cross::Resource &resource )
{
    SpvDecoration decoration { };

    decoration.set = compiler.get_decoration( resource.id, spv::DecorationDescriptorSet );
    decoration.type = compiler.get_type( resource.type_id );

    if ( decoration.type.basetype == spirv_cross::SPIRType::Struct )
    {
        uint32_t memberCount = compiler.get_declared_struct_size( decoration.type );
        // Doesn't seem necessary at the moment:
        // uint32_t m1_size = compiler.get_declared_struct_member_size( decoration.type, 0 );
        decoration.size = memberCount;
    }

    decoration.location = compiler.get_decoration( resource.id, spv::DecorationLocation );
    decoration.binding = compiler.get_decoration( resource.id, spv::DecorationBinding );

    uint32_t totalArraySize = 0;

    for ( uint32_t dimensionSize : decoration.type.array )
    {
        totalArraySize += dimensionSize;
    }

    decoration.arraySize = totalArraySize == 0 ? 1 : decoration.size / totalArraySize;
    decoration.name = resource.name;

    ensureSetExists( decoration.set );

    return decoration;
}

END_NAMESPACES