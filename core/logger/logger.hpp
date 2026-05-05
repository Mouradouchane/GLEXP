#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/bitset.hpp"

#ifndef _LOGGER_
	#define _LOGGER_ nullptr
#endif

/*

	core logger : used to log "info , errors , warns , ..." to "console , files , gui , ..."
	
	todo[future] : add log info to gui !
*/

#ifndef FMT_USE_NONTYPE_TEMPLATE_ARGS 
#define FMT_USE_NONTYPE_TEMPLATE_ARGS 0
#endif

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
#include <source_location>

namespace core {

	namespace logger {

		// used to choose what extra stuff to get printed
		// note: you can combine them using bit-operations 
		enum log_config : u8 {
			none = 0,
			line_of_code,
			file_name,
			file_path,
			// time_stamp,
			dump_stack_trace,
			function_definition,
			full_function_definition
		};

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

	#ifdef DEBUG
		DLL_API void set_level(logger::verbosity_level level);
	#endif

		// logger public function's
		template<typename... parameters> 
		void fatal(core::logger::log_config config, STRING const& format, parameters&&... params) {

			if (_LOGGER_) {
				_LOGGER_->critical(format, std::forward<parameters>(params)...);
			}
			else {
				spdlog::critical(format, std::forward<parameters>(params)...);
			}
		}

		template<typename... parameters> 
		void error(core::logger::log_config config, STRING const& format, STRING const& message) {

		}

		template<typename... parameters> 
		void warn(core::logger::log_config config, STRING const& format, STRING const& message) NOEXP {

		}

		template<typename... parameters> 
		void info(core::logger::log_config config, STRING const& format, STRING const& message) NOEXP {

		}

		template<typename... parameters> 
		void debug(core::logger::log_config config, STRING const& format, STRING const& message) NOEXP {

		}

		template<typename... parameters> 
		void trace(core::logger::log_config config, STRING const& format, STRING const& message) NOEXP {

		}

	} // namespace logger end

} // namespace core end

/*
	few macros for logger
*/ 
#define CORE_GET_LOGGER(LOGGER_NAME) spdlog::get(LOGGER_NAME)
#define CORE_GET_LOGGER_VAR(VAR_NAME, LOGGER_NAME) static auto VAR_NAME = spdlog::get(LOGGER_NAME);

#define CORE_ENABLE_LOGGER( LOGGER_VAR, LOGGER_NAME) LOGGER_VAR = spdlog::get(LOGGER_NAME);

#define CORE_ENABLE_LOGGER_IF(TRUE_CONDITION, LOGGER_VAR, LOGGER_NAME) \
		if(TRUE_CONDITION) LOGGER_VAR = spdlog::get(LOGGER_NAME); \
		else LOGGER_VAR = nullptr;

#define CORE_DISABLE_LOGGER(LOGGER_VAR) LOGGER_VAR = nullptr;

/*
	just few helper macros fatal
*/
#define CORE_LOG_DETAILS(TYPE,CONFIG) { \
			auto logger = (_LOGGER_) ? _LOGGER_ : spdlog::default_logger();\
			std::source_location location = std::source_location::current();\
			STRING second_msg = "";\
			if( GET_BIT(config , core::logger::log_config::file_name) ) {\
				second_msg += location.file_name();\
				second_msg += ", ";\
			}\
			if( GET_BIT(config , core::logger::log_config::file_path) ){\
				second_msg += __FILE__;\
				second_msg += ", ";\
			}\
			if (GET_BIT(config , core::logger::log_config::line_of_code)) {\
				second_msg += __LINE__;\
				second_msg += ", ";\
			}\
			logger->TYPE(second_msg);\
			if( GET_BIT(config , core::logger::log_config::dump_stack_trace ) ) {\
				logger->dump_backtrace();\
			}\
		}

#define CORE_LOG_HEADER(TYPE,CONFIG) {\
			auto logger = (_LOGGER_) ? _LOGGER_ : spdlog::default_logger();\
			if( GET_BIT(config , core::logger::log_config::function_definition) ) { \
				logger->TYPE(FUNCTION_DEFINITION); \
			}\
			if( GET_BIT(config , core::logger::log_config::full_function_definition) ) { \
				logger->TYPE(FUNCTION_DEFINITION_FULL); \
			}\
		}


/*
	============ FATAL macros ================
*/

#define CORE_FATAL(CONFIG, FORMAT , ...) {\
			CORE_LOG_HEADER(fatal, CONFIG); \
			auto logger = (_LOGGER_) ? _LOGGER_ : spdlog::default_logger(); \
			logger->fatal(FORMAT, ##__VA_ARGS__); \
			CORE_LOG_DETAILS(fatal, CONFIG);\
		}

#define CORE_FATAL_D(FORMAT , ...) {\
			u8 cfg = core::logger::log_config::function_definition | core::logger::log_config::dump_stack_trace;\
			CORE_FATAL(cfg , FORMAT , ##__VA_ARGS__);\
		}

#define CORE_FATAL_F(FORMAT , ...){\
			u8 cfg = core::logger::log_config::full_function_definition | core::logger::log_config::dump_stack_trace;\
			CORE_FATAL(cfg , FORMAT , ##__VA_ARGS__);\
		}


#define CORE_FATAL_IF(TRUE_EXPRESSION , CONFIG, FORMAT,  ...) \
		if(TRUE_EXPRESSION) { \
			u8 cfg = core::logger::log_config::function_definition | core::logger::log_config::dump_stack_trace;\
			CORE_FATAL(CONFIG , FORMAT , ##__VA_ARGS__);\
		}


/*
	============ error macros ================
*/

#define CORE_ERROR(CONFIG, FORMAT , ...) {\
			CORE_LOG_HEADER(error, CONFIG); \
			auto logger = (_LOGGER_) ? _LOGGER_ : spdlog::default_logger(); \
			logger->error(FORMAT, ##__VA_ARGS__); \
			CORE_LOG_DETAILS(error, CONFIG);\
		}

#define CORE_ERROR_D(FORMAT , ...) {\
			u8 cfg = core::logger::log_config::function_definition | core::logger::log_config::dump_stack_trace;\
			CORE_ERROR(cfg , FORMAT , ##__VA_ARGS__);\
		}

#define CORE_ERROR_F(FORMAT , ...){\
			u8 cfg = core::logger::log_config::full_function_definition | core::logger::log_config::dump_stack_trace;\
			CORE_ERROR(cfg , FORMAT , ##__VA_ARGS__);\
		}

#define CORE_ERROR_IF(TRUE_EXPRESSION , CONFIG, FORMAT,  ...) \
		if(TRUE_EXPRESSION) { \
			u8 cfg = core::logger::log_config::function_definition | core::logger::log_config::dump_stack_trace;\
			CORE_ERROR(CONFIG , FORMAT , ##__VA_ARGS__);\
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

	#define CORE_TRACE_CURRENT_FUNCTION() \
			if(_LOGGER_) CORE_TRACE("{} | {} | {}" , __LINE__ , FUNCTION_DEFINITION , __FILE__);

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