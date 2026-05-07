#pragma once

#ifndef LOGGER_CPP
#define LOGGER_CPP

#include <string>

#include "core/assert.hpp"
#include "logger.hpp"

#define CREATE_LOGGER(NAME, CONSOLE_SINK) { \
			auto __logger__ = std::make_shared<spdlog::logger>(NAME, CONSOLE_SINK); \
			spdlog::register_logger(__logger__); \
			__logger__->enable_backtrace(back_trace_level);\
		};

static auto _c_lg_cp = CORE_GET_DEFAULT_LOGGER();
#define _LOGGER_ _c_lg_cp

/*
	logger private variables
*/
static u32 back_trace_level = 32;
static core::logger::verbosity_level logger_level = core::logger::verbosity_level::none;

// logger
static std::shared_ptr<spdlog::logger> spd_logger;
// sinks
static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;

static bool initilized = false;

static const u16 console_width  = 800u;
static const u16 console_height = 600u;
static const u16 console_buffer_width  = 200u;
static const u16 console_buffer_height = 300u;

namespace core {

	namespace logger {

		DLL_API void init(STRING const& logger_name, verbosity_level level, u32 trace_level) {
			
			if (initilized) return;

			// allocate console + its setup
			#ifdef WINDOWS
				AllocConsole();

				HANDLE  console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
				SMALL_RECT window_size = {0, 0, (short)(console_width - 1), (short)(console_height - 1)};
				COORD      buffer_size = {(short)console_buffer_width, (short)console_buffer_height};

				SetConsoleScreenBufferSize(console_handle, buffer_size);
				SetConsoleWindowInfo(console_handle, TRUE, &window_size);

			#elif LINUX
				// todo: create console for linux
				COMPILE_TIME_ASSERT(true, "core::logger::init() --> create console in linux not implemented yet !");
				std::cout << "\e[8;" << console_height << ";" << console_width << "t" << std::flush;
			#else 
				COMPILE_TIME_ASSERT(true, "core::logger::init() --> unsupported operation system !");
			#endif

			// create sinks
			console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			file_sink    = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/" + logger_name + "_.log", 0, 0);
			std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};

			// create logger
			spd_logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin() , sinks.end());

			// register logger
			spdlog::register_logger(spd_logger);
			spdlog::set_default_logger(spd_logger);

			// setup logger sink's 
			file_sink->set_level(spdlog::level::level_enum(core::logger::verbosity_level::warning));
			file_sink->set_pattern("[%D] [%n] [%l] : %v\n");
			console_sink->set_color_mode(spdlog::color_mode::always);
			console_sink->set_pattern("%^[%l] %v%$");

			console_sink->set_color(spdlog::level::trace, 8);
			console_sink->set_color(spdlog::level::debug, 11);
			console_sink->set_color(spdlog::level::info, 10);

			spdlog::flush_on(spdlog::level::err);

			// create core sub-logger's
			CREATE_LOGGER(EVENT_SYSTEM_LOGGER,	   console_sink);
			CREATE_LOGGER(WORK_SYSTEM_LOGGER,      console_sink);
			CREATE_LOGGER(MEMORY_ALLOCATOR_LOGGER, console_sink);
			CREATE_LOGGER(DATA_STRUCTER_LOGGER,    console_sink);
			CREATE_LOGGER(REFS_LOGGER,             console_sink);

			// setup logger
			#ifdef DEBUG
				logger_level = level;
				back_trace_level = trace_level;

				// spd_logger->enable_backtrace(back_trace_level);
				spd_logger->set_level(spdlog::level::level_enum(level));
			#else 
				back_trace_level = 0;
				logger_level = core::logger::verbosity_level::error;
				spd_logger->set_level(spdlog::level::level_enum::err);
			#endif

			// unit-test logger test
			#if defined(UNIT_TEST)
				spd_logger->trace("init-logger test trace {} !" , 1);
				spd_logger->debug("init-logger test debug {} !" , 1);
				spd_logger->info("init-logger test info {} !" , 1);
				spd_logger->warn("init-logger test warn {} !" , 1);
				spd_logger->error("init-logger test error {} !" , 1);
				spd_logger->critical("init-logger test critical {} !" , 1);
			#endif

			initilized = true;
		}

		/*
			logger public function's
		*/

		DLL_API verbosity_level get_level() {
			return logger_level;
		}

		DLL_API DEBUG_ONLY void set_level(verbosity_level level) {
		#ifdef DEBUG
			spdlog::set_level(spdlog::level::level_enum(level));
		#endif
		}

		// logger public function's
		DLL_API void fatal(core::logger::log_config config, STRING&& format, STRING const& message) {
			CORE_FATAL(config, format, message);
		}
		
		DLL_API void error(core::logger::log_config config, STRING&& format, STRING const& message) {
			CORE_ERROR(config, format , message);
		}
	
	#ifdef DEBUG

		DLL_API DEBUG_ONLY void warn(core::logger::log_config config, STRING&& format, STRING const& message) NOEXP {
			CORE_WARN(config, format, message);
		}
		DLL_API DEBUG_ONLY void debug(core::logger::log_config config, STRING&& format, STRING const& message) NOEXP {
			CORE_DEBUG(config, format, message);
		}

		DLL_API DEBUG_ONLY void info(STRING&& format, STRING const& message) NOEXP {
			CORE_INFO(format, message);
		}

		DLL_API DEBUG_ONLY void trace( STRING&& format, STRING const& message) NOEXP {
			CORE_TRACE(format, message);
		}

	#endif

	} // logger namespace end

} // core namespace end

#endif 