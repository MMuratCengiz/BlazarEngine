#include "Logger.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

using namespace boost::log;

Logger::Logger( const LoggerType& loggerType ) : loggerType( loggerType )
{
    switch ( loggerType )
    {
    case LoggerType::Text:
        break;
    case LoggerType::Debug:
        break;
    case LoggerType::Console:
        boost::parameter::keyword<keywords::tag::format>::get( ) = "%Timestamp% %Message%";
        break;
    }
}

void Logger::log( const Verbosity& verbosity, const std::string& message )
{
    switch ( loggerType )
    {
        case LoggerType::Text:
            textLog( verbosity, message );
            break;
        case LoggerType::Debug:
            debugLog( verbosity, message );
            break;
        case LoggerType::Console:
            consoleLog( verbosity, message );
            break;
    }
}

void Logger::textLog( const Verbosity& verbosity, const std::string& message )
{
    
}

void Logger::debugLog( const Verbosity& verbosity, const std::string& message )
{
    
}

void Logger::consoleLog( const Verbosity& verbosity, const std::string& message ) const
{
    BOOST_LOG_TRIVIAL( info ) << message;
}

END_NAMESPACES