#pragma once

#include <BlazarCore/Common.h>
#include <BlazarInput/GlobalEventHandler.h>

#ifdef WIN32
#include <windows.h>
#endif

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class RenderWindow
{
private:
#ifdef WIN32
    HINSTANCE hInstance;
    HWND hWindow;
#endif
    uint32_t width;
    uint32_t height;

    std::vector< std::string > extensions;
public:
    RenderWindow( const uint32_t& width, const uint32_t& height, std::vector< std::string > extensions ) : width( width ), height( height ), extensions( std::move( extensions ) )
    {
        Input::GlobalEventHandler::Instance().subscribeToEvent(
                Input::EventType::WindowResized, [ & ]( const Input::EventType& eventType, std::shared_ptr< Input::IEventParameters > parameters )
                {
                    auto windowResizeParameters = Input::GlobalEventHandler::ToWindowResizedParameters( parameters );

                    this->width  = windowResizeParameters->width;
                    this->height = windowResizeParameters->height;
                });
    }

    inline const uint32_t& getWidth( ) const
    {
        return width;
    }

    inline const uint32_t& getHeight( ) const
    {
        return height;
    }

    inline const std::vector< std::string >& getExtensions( ) const
    {
        return extensions;
    }

#ifdef WIN32
    void setPlatformSpecific( HINSTANCE instance, HWND window )
    {
        this->hInstance = instance;
        this->hWindow = window;
    }

    const HINSTANCE getHInstance( ) const
    {
        return hInstance;
    }

    const HWND getHWindow( ) const
    {
        return hWindow;
    }
#endif
};

END_NAMESPACES

