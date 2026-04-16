#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#ifndef _LOGGER_
	#define _LOGGER_ nullptr
#endif
/*

	core logger : used to log "info , errors , warns , ..." to "console , files , gui , ..."
	
*/

// todo[future] : add log info to gui !


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

#include "core/strings/string.hpp"
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

		DLL_API void init(STRING const& logger_name, logger::verbosity_level level, u32 trace_level = 32);

		DLL_API logger::verbosity_level get_level();
		// works in debug only !
		DLL_API void set_level(logger::verbosity_level level);

		// logger public function's
		DLL_API void fatal(STRING const& message);
		DLL_API void error(STRING const& message);
		DLL_API void warn (STRING const& message);
		DLL_API void info (STRING const& message);
		DLL_API void debug(STRING const& message);
		DLL_API void trace(STRING const& message);

	} // namespace logger end

} // namespace core end

#define CORE_GET_LOGGER(LOGGER_NAME) spdlog::get(LOGGER_NAME)
#define CORE_GET_LOGGER_VAR(VAR_NAME, LOGGER_NAME) static auto VAR_NAME = spdlog::get(LOGGER_NAME);

#define CORE_ENABLE_LOGGER( LOGGER_VAR, LOGGER_NAME) LOGGER_VAR = spdlog::get(LOGGER_NAME);

#define CORE_ENABLE_LOGGER_IF(TRUE_CONDITION, LOGGER_VAR, LOGGER_NAME) \
		if(TRUE_CONDITION) LOGGER_VAR = spdlog::get(LOGGER_NAME); \
		else LOGGER_VAR = nullptr;

#define CORE_DISABLE_LOGGER(LOGGER_VAR) LOGGER_VAR = nullptr;

/*
	logger macros functions
*/

// log fatals/errors
#define CORE_FATAL(FORMAT , ...)   \
		if(_LOGGER_) _LOGGER_->critical(FORMAT , ##__VA_ARGS__); \
		else           spdlog::critical(FORMAT , ##__VA_ARGS__);

#define CORE_FATAL_D(FORMAT , ...) \
		if(_LOGGER_){ _LOGGER_->critical(FUNCTION_DEFINITION); _LOGGER_->critical(FORMAT , ##__VA_ARGS__); } \
		else {          spdlog::critical(FUNCTION_DEFINITION);   spdlog::critical(FORMAT , ##__VA_ARGS__); }

#define CORE_FATAL_F(FORMAT , ...) \
		if(_LOGGER_){ _LOGGER_->critical(FUNCTION_DEFINITION_FULL); _LOGGER_->critical(FORMAT , ##__VA_ARGS__); } \
		else{           spdlog::critical(FUNCTION_DEFINITION_FULL);   spdlog::critical(FORMAT , ##__VA_ARGS__); }

#define CORE_ERROR(FORMAT , ...)   \
		if(_LOGGER_) _LOGGER_->error(FORMAT , ##__VA_ARGS__); \
		else           spdlog::error(FORMAT , ##__VA_ARGS__);

#define CORE_ERROR_D(FORMAT , ...) \
		if(_LOGGER_){ _LOGGER_->error(FUNCTION_DEFINITION); _LOGGER_->error(FORMAT , ##__VA_ARGS__); } \
		else {          spdlog::error(FUNCTION_DEFINITION);   spdlog::error(FORMAT , ##__VA_ARGS__); }

#define CORE_ERROR_F(FORMAT , ...) \
		if(_LOGGER_){ _LOGGER_->error(FUNCTION_DEFINITION_FULL); _LOGGER_->error(FORMAT , ##__VA_ARGS__); } \
		else {          spdlog::error(FUNCTION_DEFINITION_FULL);   spdlog::error(FORMAT , ##__VA_ARGS__); }

// log errors if true
#define CORE_FATAL_IF(TRUE_EXPRESSION ,FORMAT ,  ...) \
		if(TRUE_EXPRESSION) { \
			if(_LOGGER_) _LOGGER_->critical(FORMAT , ##__VA_ARGS__); \
			else           spdlog::critical(FORMAT , ##__VA_ARGS__); \
		}

#define CORE_ERROR_IF(TRUE_EXPRESSION ,FORMAT ,  ...) \
		if(TRUE_EXPRESSION) { \
			if(_LOGGER_) _LOGGER_->error(FORMAT , ##__VA_ARGS__); \
			else           spdlog::error(FORMAT , ##__VA_ARGS__); \
		}

#ifdef DEBUG // debug-only functions
	#define CORE_WARN(FORMAT , ...)  if(_LOGGER_) _LOGGER_->warn( FORMAT , ##__VA_ARGS__);
	#define CORE_INFO(FORMAT , ...)  if(_LOGGER_) _LOGGER_->info( FORMAT , ##__VA_ARGS__);
	#define CORE_DEBUG(FORMAT , ...) if(_LOGGER_) _LOGGER_->debug(FORMAT , ##__VA_ARGS__);
	#define CORE_TRACE(FORMAT , ...) if(_LOGGER_) _LOGGER_->trace(FORMAT , ##__VA_ARGS__);

	#define CORE_WARN_D(FORMAT  , ...) if(_LOGGER_){ _LOGGER_->warn(FUNCTION_DEFINITION);  _LOGGER_->warn(FORMAT, ##__VA_ARGS__); }
	#define CORE_INFO_D(FORMAT  , ...) if(_LOGGER_){ _LOGGER_->info(FUNCTION_DEFINITION);  _LOGGER_->info(FORMAT, ##__VA_ARGS__); }
	#define CORE_DEBUG_D(FORMAT , ...) if(_LOGGER_){ _LOGGER_->debug(FUNCTION_DEFINITION); _LOGGER_->debug(FORMAT, ##__VA_ARGS__);}
	#define CORE_TRACE_D(FORMAT , ...) if(_LOGGER_){ _LOGGER_->trace(FUNCTION_DEFINITION); _LOGGER_->trace(FORMAT, ##__VA_ARGS__);}

	// log if true
	#define CORE_WARN_IF(TRUE_EXPRESSION, FORMAT,  ...) if(TRUE_EXPRESSION) if(_LOGGER_) _LOGGER_->warn(FORMAT , ##__VA_ARGS__);
	#define CORE_INFO_IF(TRUE_EXPRESSION, FORMAT,  ...) if(TRUE_EXPRESSION) if(_LOGGER_) _LOGGER_->info(FORMAT , ##__VA_ARGS__);

	#define CORE_TRACE_CURRENT_FUNCTION() if(_LOGGER_) CORE_TRACE("{} | {} | {}" , __LINE__ , FUNCTION_DEFINITION , __FILE__);

#else 
	#define CORE_WARN(...)
	#define CORE_INFO(...)
	#define CORE_DEBUG(...)
	#define CORE_TRACE(...)

	#define CORE_WARN_D(...)
	#define CORE_INFO_D(...)
	#define CORE_DEBUG_D(...)
	#define CORE_TRACE_D(...)

	#define CORE_WARN_IF(TRUE_EXPRESSION , ...) 
	#define CORE_INFO_IF(TRUE_EXPRESSION , ...)

	#define CORE_TRACE_CURRENT_FUNCTION(...) 
#endif

#endif 