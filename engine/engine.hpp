#pragma once 

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

/*
	few macros for errors and warnings for engine
*/
#define ENGINE_INIT_FAILED   "engine failed to init because of {} ."
#define ENGINE_RUNTIME_CRASH "engine run-time crash because of {} ."
#define ENGINE_ALREADY_INIT  "engine is already initilized and ready to use !"

namespace engine {

	// engine run-time status
	enum class status : u16 {
		stoping = 0,
		pausing,
		loading,
		running,
		waiting,
	};

	bool init()     NOEXP;
	void run()      NOEXP;
	void shutdown() NOEXP;

}

#endif