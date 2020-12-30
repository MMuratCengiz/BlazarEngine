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

    enum class Input
    {
        GeometryData,
        ViewProjection,
        SkyBox,
        EnvironmentLights,
        Material,
        ModelMatrix
    };

    static std::string getInputName( const Input &inputName )
    {
        switch ( inputName )
        {
            case Input::GeometryData:
                return "GeometryData";
            case Input::ViewProjection:
                return "ViewProjection";
            case Input::EnvironmentLights:
                return "EnvironmentLights";
            case Input::SkyBox:
                return "SkyBox";
            case Input::Material:
                return "Material";
            case Input::ModelMatrix:
                return "ModelMatrix";
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
