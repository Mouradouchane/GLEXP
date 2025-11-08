#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

/*
	logger --> used to log information to "console,files,gui" !
	todo: add log info to ui-console !
*/

// disable fmt & spdlog warnings
DISABLE_WARNING_START
#pragma warning(disable : 26495,26498,26812,6294,26451,4002,4006,4067);
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

		DLL_API void init(std::string const& logger_name, logger::verbosity_level level, u32 trace_level = 32);

		DLL_API logger::verbosity_level get_level();
		// works in debug only !
		DLL_API void set_level(logger::verbosity_level level);

		// logger public function's
		DLL_API void fatal(std::string const& message);
		DLL_API void error(std::string const& message);
		DLL_API void warn (std::string const& message);
		DLL_API void info (std::string const& message);
		DLL_API void debug(std::string const& message);
		DLL_API void trace(std::string const& message);

	} // namespace logger end

} // namespace core end

/*
	logger macros functions
*/

#define CORE_FATAL(...) spdlog::critical(__VA_ARGS__);
#define CORE_ERROR(...) spdlog::error(__VA_ARGS__);
#define CORE_FATAL_IF(TRUE_EXPRESSION , ...) if(TRUE_EXPRESSION) spdlog::critical(__VA_ARGS__);
#define CORE_ERROR_IF(TRUE_EXPRESSION , ...) if(TRUE_EXPRESSION) spdlog::error(__VA_ARGS__);

#ifdef DEBUG // logger debug only functions
	#define CORE_WARN(...)  spdlog::warn(__VA_ARGS__);
	#define CORE_INFO(...)  spdlog::info(__VA_ARGS__);
	#define CORE_DEBUG(...) spdlog::debug(__VA_ARGS__);
	#define CORE_TRACE(...) spdlog::trace(__VA_ARGS__);
			// log with if-condition
	#define CORE_WARN_IF(TRUE_EXPRESSION , ...) if(TRUE_EXPRESSION) spdlog::warn(__VA_ARGS__);
	#define CORE_INFO_IF(TRUE_EXPRESSION , ...) if(TRUE_EXPRESSION) spdlog::info(__VA_ARGS__);

#else 
	#define CORE_WARN(...)
	#define CORE_INFO(...)
	#define CORE_DEBUG(...)
	#define CORE_TRACE(...)
	
	#define CORE_WARN_IF(TRUE_EXPRESSION , ...) 
	#define CORE_INFO_IF(TRUE_EXPRESSION , ...)
#endif

#endif 