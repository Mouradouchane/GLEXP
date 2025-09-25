#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

/*
	here is the core logger as namespace
	logger used by core it's self and all other projects
*/

#include "core/macros.hpp"
#include "core/types.hpp"

// todo : replace std::string with our string 
#include <string>
#include <stdarg.h>

// used to control logger verbosity level
enum class logger_verbosity_level : u8 {
	trace = 0, 
	debug,	
	info,
	warning, 
	error,
	fatal,
	none, // no messages at all
};

namespace core {

namespace logger {

	void init(logger_verbosity_level logger_verbosity_level, u32 backtrace_level = 32);

	// logger public function's
	inline void fatal(std::string const& message);
	inline void error(std::string const& message);
	inline void warn (std::string const& message);
	inline void info (std::string const& message);
	inline void debug(std::string const& message);
	inline void trace(std::string const& message);

}; // namespace logger end

} // namespace core end

/*
	logger macros functions
*/

#define CORE_FATAL(...) spdlog::critical(__VA_ARGS__)
#define CORE_ERROR(...) spdlog::error(__VA_ARGS__)

#ifdef DEBUG // logger debug only functions
	#define CORE_WARN(...)  spdlog::warn(__VA_ARGS__)
	#define CORE_INFO(...)  spdlog::info(__VA_ARGS__)
	#define CORE_TRACE(...) spdlog::trace(__VA_ARGS__)
#else 
	#define CORE_WARN(...)
	#define CORE_INFO(...)
	#define CORE_TRACE(...)
#endif

#endif 