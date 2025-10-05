#pragma once

#ifndef LOGGER_CPP
#define LOGGER_CPP

#include <string>

#include "core/errors/assert.hpp"
#include "logger.hpp"

/*
	logger private variables
*/
static u32 back_trace_level = 32;
static core::logger::verbosity_level  logger_level = core::logger::verbosity_level::none;

// logger
static std::shared_ptr<spdlog::logger> spd_logger;
// sinks
static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;

static bool initilized = false;

namespace core {

	namespace logger {

		DLL_EXPORT void init(std::string const& logger_name, verbosity_level level, u32 trace_level) {
			
			if (initilized) return;

			#ifdef WINDOWS
				AllocConsole();
			#elif LINUX
				// todo: create console for linux
				COMPILE_TIME_ASSERT(0, "core::logger::init --> create console in linux not implemented yet !");
			#else 
				COMPILE_TIME_ASSERT(0, "core::logger::init --> unsupported operation system !");
			#endif

			// create sinks
			console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			file_sink    = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/" + logger_name + "_.log", 0, 0);
			std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};
			
			// setup sinks 
			file_sink->set_level(spdlog::level::level_enum(core::logger::verbosity_level::warning));
			file_sink->set_pattern("[%T] [%l] %v \n");
			console_sink->set_pattern("%^[%l] %v %$");

			// create logger
			spd_logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin() , sinks.end());
			spdlog::set_default_logger(spd_logger);

			// setup logger
			#ifdef DEBUG
				logger_level = level;
				back_trace_level = trace_level;

				spdlog::enable_backtrace(back_trace_level);
				spdlog::set_level(spdlog::level::level_enum(level));
			#else 
				back_trace_level = 0;
				logger_level = logger_verbosity_level::error;
				spdlog::set_level(spdlog::level::level_enum::err);
			#endif

			initilized = true;

			// testing logger
			#if 0
				spd_logger->info("testing info  -> {}", 1);
				spd_logger->warn("testing warn  -> {}", 1);
				spd_logger->debug("testing debug -> {}", 1);
				spd_logger->trace("testing trace -> {}", 1);
				spd_logger->error("testing error -> {}", 1);
				spd_logger->critical("testing fatal -> {}", 1);
			#endif
		}

		/*
			logger public function's
		*/

		DLL_EXPORT verbosity_level get_level() {
			return logger_level;
		}

		DLL_EXPORT void set_level(verbosity_level level) {
		#ifdef DEBUG
			spdlog::set_level(spdlog::level::level_enum(level));
		#endif
		}

		DLL_EXPORT inline void fatal(std::string const& message) {
			spdlog::critical(message);
		}

		DLL_EXPORT inline void error(std::string const& message) {
			spdlog::error(message);
		}

		DLL_EXPORT inline void warn(std::string const& message) {
		#ifdef DEBUG
			spdlog::warn(message);
		#endif
		}

		DLL_EXPORT inline void  info(std::string const& message) {
		#ifdef DEBUG
			spdlog::info(message);
		#endif
		}

		DLL_EXPORT inline void  debug(std::string const& message) {
		#ifdef DEBUG
			spdlog::debug(message);
		#endif
		}

		DLL_EXPORT inline void  trace(std::string const& message) {
		#ifdef DEBUG
			spdlog::trace(message);
		#endif
		}

	} // logger namespace end

} // core namespace end

#endif 