#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

/*
	here is the core logger as namespace
	logger used by core it's self and all other projects
*/

#include "core/macros.hpp"
#include "core/types.hpp"

// disable all spdlog warnings
#pragma warning(push, 0) 
	#include "libs/spdlog/spdlog.h"
	#include "libs/spdlog/fmt/ostr.h"
#pragma warning(pop)

#include <stdarg.h>
// todo : replace std::string with our string 
#include <string>

namespace core {

namespace logger {

	// used to control logger verbosity level
	enum class verbosity_level : u8 {
		disable = 0, // no messages at all
		error,
		warning, 
		info, 
		debug	
	};

	// logger 
	enum class log_level : u8 {
		fatal = 0, // errors who cause crash
		error, // errors who can be handled 
		/*
			note: in debug only !
		*/
		warning, // when something is wrong !
		info,
		// debug information 
		debug, 
		trace	
	};

	// logger public function's
	inline void fatal(std::string const& message , ...);
	inline void error(std::string const& message , ...);
	inline void warn( std::string const& message , ...);
	inline void info( std::string const& message , ...);
	inline void debug(std::string const& message , ...);
	inline void trace(std::string const& message , ...);

}; // namespace logger end

} // namespace core end

/*
	logger macros functions
*/

#define CORE_FATAL(MESSAGE , ...) core::logger::fatal(MESSAGE , __VA_ARGS__)
#define CORE_ERROR(MESSAGE , ...) core::logger::error(MESSAGE , __VA_ARGS__)

#ifdef DEBUG // logger debug only functions
	#define CORE_WARN(MESSAGE  , ...) core::logger::warn(MESSAGE  , __VA_ARGS__)
	#define CORE_INFO(MESSAGE  , ...) core::logger::info(MESSAGE  , __VA_ARGS__)
	#define CORE_TRACE(MESSAGE , ...) core::logger::trace(MESSAGE , __VA_ARGS__)
#else 
	#define CORE_WARN(MESSAGE  , ...)
	#define CORE_INFO(MESSAGE  , ...)
	#define CORE_TRACE(MESSAGE , ...)
#endif


#endif 