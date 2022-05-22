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

class StaticVars
{
public:
    enum class Attachment
    {
        Color,
        Depth
    };

    enum class Output
    {
        SwapChainImage
    };

    enum class ShaderInput
    {
        GeometryData,
        ViewProjection,
        SkyBox,
        EnvironmentLights,
        Material,
        ModelMatrix,
        NormalModelMatrix
    };

    static std::string getInputName( const ShaderInput &inputName )
    {
        switch ( inputName )
        {
            case ShaderInput::GeometryData:
                return "GeometryData";
            case ShaderInput::ViewProjection:
                return "ViewProjection";
            case ShaderInput::EnvironmentLights:
                return "EnvironmentLights";
            case ShaderInput::SkyBox:
                return "SkyBox";
            case ShaderInput::Material:
                return "Material";
            case ShaderInput::ModelMatrix:
                return "ModelMatrix";
            case ShaderInput::NormalModelMatrix:
                return "NormalModelMatrix";
        }

        return "";
    }

    static std::string getOutputName( const Output &outputName )
    {
        switch ( outputName )
        {
            case Output::SwapChainImage:
                return "SwapChainImage";
        }

        return "";
    }

    static std::string getAttachmentName( const Attachment &attachmentName )
    {
        switch ( attachmentName )
        {
            case Attachment::Color:
                return "Color";
            case Attachment::Depth:
                return "Depth";
        }

        return "";
    }
};

END_NAMESPACES
