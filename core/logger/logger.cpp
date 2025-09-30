#pragma once

#ifndef LOGGER_CPP
#define LOGGER_CPP

#include "logger.hpp"
#include "core/errors/assert.hpp"

/*
	logger private variables
*/
static u32 back_trace_level = 32;
static logger_verbosity_level logger_level = logger_verbosity_level::none;

void core::logger::init(logger_verbosity_level level , u32 trace_level) {

#ifdef WINDOWS
	AllocConsole();
#elif LINUX
	// todo: create console for linux
	COMPILE_TIME_ASSERT(0,"core::logger::init --> create console in linux not implemented yet !");
#else 
	COMPILE_TIME_ASSERT(0,"core::logger::init --> unsupported operation system !");
#endif

#ifdef DEBUG
	logger_level = level;
	back_trace_level = trace_level;
	
	spdlog::enable_backtrace(back_trace_level);
	spdlog::set_level( spdlog::level::level_enum(level) );
#else 
	back_trace_level = 0;
	logger_level = logger_verbosity_level::error;
	spdlog::set_level( spdlog::level::level_enum::err );
#endif

	spdlog::set_pattern("%^[%T] [%l] %v %$");

}

logger_verbosity_level core::logger::get_level() {
	return logger_level;
}

void core::logger::set_level(logger_verbosity_level level) {
#ifdef DEBUG
	spdlog::set_level( spdlog::level::level_enum(level) );
#endif
}

// logger public function's
inline void core::logger::fatal(std::string const& message){
	spdlog::critical(message);
}

inline void core::logger::error(std::string const& message ){
	spdlog::error(message);
}

inline void core::logger::warn( std::string const& message ){
#ifdef DEBUG
	spdlog::warn(message);
#endif
}

inline void core::logger::info( std::string const& message ){
#ifdef DEBUG
	spdlog::info(message);
#endif
}

inline void core::logger::debug(std::string const& message ){
#ifdef DEBUG
	spdlog::debug(message);
#endif
}

inline void core::logger::trace(std::string const& message ){
#ifdef DEBUG
	spdlog::trace(message);
#endif
}

#endif 