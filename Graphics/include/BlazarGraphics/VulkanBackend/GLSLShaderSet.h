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
#include "../GraphicsCommonIncludes.h"
#include "SpirvHelper.h"
#include "BlazarCore/Utilities.h"
#include "spirv_glsl.hpp"
#include <BlazarGraphics/IShaderInfo.h>

/*
TODO List:
- Support matrix as vertex input.
*/
NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct DescriptorSetBinding
{
    uint16_t index;
    vk::DescriptorType type;
    vk::DescriptorSetLayoutBinding layout;
    vk::DeviceSize size;
    std::string name;
};

struct DescriptorSet
{
    uint32_t id;
    std::vector< vk::DescriptorSetLayoutBinding > descriptorSetLayoutBindings;
    std::vector< DescriptorSetBinding > descriptorSetBindings;
    std::unordered_map< std::string, DescriptorSetBinding > descriptorSetBindingMap;
};

struct GLSLShaderInfo
{
    vk::ShaderStageFlagBits type;
    std::vector< uint32_t > data;
public:
    GLSLShaderInfo( ShaderType type, const std::string& path ) : GLSLShaderInfo( genericTypeToVkType( type ), path ){ }

    GLSLShaderInfo( vk::ShaderStageFlagBits type, const std::string& path )
    {
        this->type = type;
        auto glslContents = Core::Utilities::readFile( path ) ;
        data = SpirvHelper::GLSLtoSPV( type, glslContents.c_str( ) );
    }

    static vk::ShaderStageFlagBits genericTypeToVkType( ShaderType type )
    {
        vk::ShaderStageFlagBits shaderType { };

        switch ( type )
        {
            case ShaderType::Vertex:
                shaderType = vk::ShaderStageFlagBits::eVertex;
                break;
            case ShaderType::Fragment:
                shaderType = vk::ShaderStageFlagBits::eFragment;
                break;
            case ShaderType::Geometry:
                shaderType = vk::ShaderStageFlagBits::eGeometry;
                break;
            case ShaderType::TessellationControl:
                shaderType = vk::ShaderStageFlagBits::eTessellationControl;
                break;
            case ShaderType::TessellationEval:
                shaderType = vk::ShaderStageFlagBits::eTessellationEvaluation;
                break;
        }

        return shaderType;
    }
};

struct StructChild
{
    std::string name;
    uint32_t offset;
    uint32_t size;
};

struct PushConstantDetail
{
    vk::ShaderStageFlagBits stage;
    uint32_t size;
    std::string name;
    std::vector< StructChild > children;
};

class GLSLShaderSet : public IShaderInfo
{
private:
    struct SpvDecoration
    {
        spirv_cross::SPIRType type;
        uint32_t set;
        uint32_t location;
        uint32_t binding;
        uint32_t arraySize;
        uint32_t size;
        std::string name;
        std::vector< StructChild > children;
    };

    struct GLSLType
    {
        vk::Format format;
        uint32_t size;
    };

    struct DescriptorBindingCreateInfo
    {
        uint32_t binding { };
        spirv_cross::Resource resource;
        vk::DescriptorType type;
        vk::ShaderStageFlagBits stage;
    };

    std::vector< vk::VertexInputBindingDescription > inputBindingDescriptions;
    std::vector< vk::VertexInputAttributeDescription > vertexAttributeDescriptions;
    std::unordered_map< uint32_t, DescriptorSet > descriptorSetMap;
    std::vector< DescriptorSet > descriptorSets;
    std::vector< vk::PushConstantRange > pushConstants;
    std::vector< PushConstantDetail > pushConstantDetails;

    bool interleavedMode;
public:
    explicit GLSLShaderSet( const std::vector< GLSLShaderInfo > &shaders, const bool &interleavedMode = true );

    inline const std::vector< DescriptorSet > &getDescriptorSets( )
    {
        return descriptorSets;
    }

    inline const DescriptorSet &getDescriptorSetBySetId( uint32_t id )
    {
        return descriptorSetMap[ id ];
    }

    inline const std::vector< vk::VertexInputBindingDescription > &getInputBindingDescriptions( )
    {
        return inputBindingDescriptions;
    }

    inline const std::vector< vk::PushConstantRange > &getPushConstants( )
    {
        return pushConstants;
    }

    inline const std::vector< PushConstantDetail > &getPushConstantDetails( )
    {
        return pushConstantDetails;
    }

    inline const std::vector< vk::VertexInputAttributeDescription > &getVertexAttributeDescriptions( )
    {
        return vertexAttributeDescriptions;
    }

    inline std::vector< std::string > getMergedInputs( ) override
    {
        std::vector< std::string > result;

        for ( const auto& sets: descriptorSets )
        {
            for ( const auto& set: sets.descriptorSetBindings )
            {
                result.emplace_back( set.name );
            }
        }

        for ( const auto& pushConstant: pushConstantDetails )
        {
            for ( const auto& pushConstantChild: pushConstant.children )
            {
                result.emplace_back( pushConstantChild.name );
            }
        }

        return std::move( result );
    }
private:
    void onEachShader( const GLSLShaderInfo &shaderInfo );
    void ensureSetExists( uint32_t set );
    void createVertexInput( const uint32_t &offset, const GLSLType &type, const uint32_t &location );

    void createDescriptorSetBinding( const spirv_cross::Compiler &compiler, const DescriptorBindingCreateInfo &bindingCreateInfo );
    static GLSLType spvToGLSLType( const spirv_cross::SPIRType &type );
    SpvDecoration getDecoration( const spirv_cross::Compiler &compiler, const spirv_cross::Resource &resource );
    void updateDecoration( const DescriptorBindingCreateInfo &bindingCreateInfo, const SpvDecoration &decoration, const DescriptorSet &descriptorSet );
};


END_NAMESPACES
