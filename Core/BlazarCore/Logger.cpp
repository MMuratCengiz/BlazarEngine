#include "Logger.h"

#include <fstream>

NAMESPACES( ENGINE_NAMESPACE, Core )

Logger::Logger( const LoggerType& loggerType ) : loggerType( loggerType )
{
    if ( loggerType == LoggerType::File )
    {
        logStream.open( "./log.txt", std::fstream::out | std::fstream::trunc ); // todo read from settings or something
    }
}

void Logger::log( const Verbosity& verbosity, const std::string& component, const std::string& message )
{
    FUNCTION_BREAK ( verbosity > globalVerbosity )

    const auto formattedMessage = boost::format("[%1%][%2%]: %3%") % component % verbosityStrMap[ static_cast< int >( verbosity ) ] % message;

    switch ( loggerType )
    {
        case LoggerType::File:
            fileLog( formattedMessage.str(  ) );
            break;
        case LoggerType::Console:
            consoleLog( formattedMessage.str(  ) );
            break;
    }
}

void Logger::fileLog( const std::string& message )
{
    messageQueue.push( message );
}

void Logger::consoleLog( const std::string& message ) const
{
    std::cout << message << std::endl;
}

void Logger::logListener( )
{
    while ( running )
    {
        while ( ! messageQueue.empty(  ) )
        {
            std::string messageToLog = messageQueue.top(  );
            logStream << messageToLog;
            messageQueue.pop( );
        }
    }
}

Logger::~Logger( )
{
    try
    {
        logStream.close( );
    } catch ( const std::exception & ) { }

    running = false;
}

END_NAMESPACES
