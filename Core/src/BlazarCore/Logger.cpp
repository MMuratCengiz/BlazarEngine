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

#include <BlazarCore/Logger.h>

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
