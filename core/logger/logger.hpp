#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

/*
	here is the core logger as namespace
	logger used by core it's self and all other projects
*/

#include "core/macros.hpp"
#include "core/types.hpp"

// disable fmt & spdlog warnings
DISABLE_WARNING_START
#pragma warning( disable : 26495,26498,26812,6294,26451,4002,4006,4067)
	#include <libs/spdlog/spdlog.h>
	#include <libs/spdlog/sinks/stdout_color_sinks.h>
	#include "spdlog/sinks/daily_file_sink.h"
	//#include <spdlog/sinks/basic_file_sink.h>
	#include <libs/spdlog/sinks/wincolor_sink.h>
	#include <libs/spdlog/fmt/ostr.h>
DISABLE_WARNING_END


// todo : replace std::string with our string 
#include <string>
#include <stdarg.h>


namespace core {

	namespace logger {

		// used to control logger verbosity level
		enum class verbosity_level : u8 {
			trace = 0, 
			debug,	
			info,
			warning, 
			error,
			fatal,
			none, // no messages at all
		};

		DLL_IMPORT void init(std::string const& logger_name, logger::verbosity_level level, u32 trace_level = 32);

		DLL_IMPORT logger::verbosity_level get_level();
		// works in debug only !
		DLL_IMPORT void set_level(logger::verbosity_level level);

		// logger public function's
		DLL_IMPORT inline void fatal(std::string const& message);
		DLL_IMPORT inline void error(std::string const& message);
		DLL_IMPORT inline void warn (std::string const& message);
		DLL_IMPORT inline void info (std::string const& message);
		DLL_IMPORT inline void debug(std::string const& message);
		DLL_IMPORT inline void trace(std::string const& message);

	} // namespace logger end

} // namespace core end

/*
	logger macros functions
*/

#define CORE_FATAL(...) spdlog::critical(__VA_ARGS__);
#define CORE_ERROR(...) spdlog::error(__VA_ARGS__);

#ifdef DEBUG // logger debug only functions
	#define CORE_WARN(...)  spdlog::warn(__VA_ARGS__);
	#define CORE_INFO(...)  spdlog::info(__VA_ARGS__);
	#define CORE_DEBUG(...) spdlog::debug(__VA_ARGS__);
	#define CORE_TRACE(...) spdlog::trace(__VA_ARGS__);
#else 
	#define CORE_WARN(...)
	#define CORE_INFO(...)
	#define CORE_DEBUG(...)
	#define CORE_TRACE(...)
#endif

#endif 