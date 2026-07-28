#pragma once

#ifndef CORE_TIMER_HPP
#define CORE_TIMER_HPP

#include <chrono>

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"

/*
	few macros for timers
*/ 
#define TIMER(FUNCTION) \
		{\
			timer t; \
			FUNCTION;\
		}

#define TIMER_ASSIGN(VAR , FUNCTION) \
		{\
			timer t; \
			VAR = FUNCTION;\
		}


// few types for time
typedef std::chrono::time_point<std::chrono::steady_clock> time_point;
typedef std::chrono::duration<f64> time_elapse;

// to tell the timer used for what !
enum class timer_tag : u8 {
	unkown = 0,

	graphics,
	audio,
	physics,
	ai,
	memory,
	events,
	animation,
	collision,
	files,
	containers,

#ifdef DEBUG
	dev,
	debug
#endif
};
// enum timer_tag end


DLL_API_CLASS timer {

private:
	time_point  start_point;
	time_elapse elapse;

	u64 index = (u64)-1;
#ifdef DEBUG
	timer_tag tag;
#endif

public:
	 timer() NOEXP = default;
	~timer() NOEXP;

	void restart() NOEXP;

	u64 nanoseconds() NOEXP;
	u64 miliseconds() NOEXP;
	f64 seconds()     NOEXP;

}; // class timer end


namespace core {

	string time_to_string(timer t) NOEXP;

}

#endif 