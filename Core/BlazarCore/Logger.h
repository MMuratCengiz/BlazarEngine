#pragma once

#include "Common.h"
#include <fstream>
#include <queue>
#include <thread>
#include <boost/format.hpp>

NAMESPACES( ENGINE_NAMESPACE, Core )

enum class LoggerType
{
	File,
	Console
};

enum class Verbosity : int
{
	Critical = 0,
	Warning = 1,
	Information = 2,
	Debug = 3
};

class Logger
{
private:

#ifdef DEBUG
	const Verbosity globalVerbosity = Verbosity::Information;
#elif 
	const Verbosity globalVerbosity = Verbosity::Critical;
#endif

	const std::string verbosityStrMap[ 4 ] = { "Critical", "Warning", "Information", "Debug" };

	LoggerType loggerType;

	std::fstream logStream;
	std::priority_queue< std::string > messageQueue;
	std::thread listener;

	bool running = true;

	explicit Logger( const LoggerType& loggerType );
public:
	static Logger& get( )
	{
		static Logger instance(
#ifdef DEBUG
			LoggerType::Console
#elif 
			LoggerType::File
#endif		
		);

		return instance;
	}

	void log( const Verbosity& verbosity, const std::string& component, const std::string& message );
	~Logger( );
private:
	void fileLog( const std::string& message );
	void consoleLog( const std::string& message ) const;
	void logListener( );
};

END_NAMESPACES

#define LOG( verbosity, component, message)  ENGINE_NAMESPACE::Core::Logger::get( ).log( verbosity, component, message )