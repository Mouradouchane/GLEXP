#pragma once

#ifndef LOGGER_CPP
#define LOGGER_CPP

#include "logger.hpp"

// disable fmt & spdlog warnings
DISABLE_WARNING_START
#pragma warning( disable : 26451,4002,4006,4067)
	#include <libs/spdlog/spdlog.h>
	#include <libs/spdlog/sinks/stdout_color_sinks.h>
	#include <libs/spdlog/fmt/ostr.h>
DISABLE_WARNING_END

static u32 back_trace_level = 32;
static spdlog::logger* spd_stdout_sink;
static spdlog::logger* xspd_logger;

void core::logger::init(logger_verbosity_level logger_verbosity_level , u32 backtrace_level) {
	
	auto spd_logger = spdlog::stdout_color_mt("core_logger");

	spdlog::set_default_logger(spd_logger);

#ifdef DEBUG
	back_trace_level = backtrace_level;
	spdlog::enable_backtrace(back_trace_level);
	
	spdlog::set_level( spdlog::level::level_enum(logger_verbosity_level) );
#else 
	spdlog::set_level( spdlog::level::level_enum::err );
#endif
	spdlog::set_pattern("%^[%T] [%l] : %v %$");

	// just a quick test if we are in debug
#ifdef DEBUG
	spd_logger->info("log info test !");
	core::logger::warn("log warn test !");
	core::logger::debug("log debug test !");
	core::logger::error("log error test !");
	core::logger::fatal("log fatal test !");
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