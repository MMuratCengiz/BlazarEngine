#pragma once

#include "Common.h"
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

NAMESPACES( ENGINE_NAMESPACE, Core )

using namespace boost::log;

enum class LoggerType
{
    Text,
	Debug,
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
	LoggerType loggerType;

	typedef sinks::asynchronous_sink<sinks::text_ostream_backend> TextSink;
	typedef sinks::asynchronous_sink<sinks::basic_debug_output_backend< char > > DebugSink;

	Logger( const LoggerType& loggerType );
public:
	static Logger & get( )
	{
		static Logger instance(
#ifdef DEBUG
			LoggerType::Console // Todo Debug later
#elif 
			LoggerType::Text
#endif		
		);

		return instance;
	}

	void log( const Verbosity& verbosity, const std::string& message );
private:
	void textLog( const Verbosity& verbosity, const std::string& message );
	void debugLog( const Verbosity& verbosity, const std::string& message );
	void consoleLog( const Verbosity& verbosity, const std::string& message ) const;
};

END_NAMESPACES