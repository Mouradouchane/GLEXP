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
#ifdef DEBUG
	// todo: implement this
	#define REGISTER_TIMER(VAR,TAG)

	#define TIMER(FUNCTION) \
			{\
				timer t; \
				FUNCTION;\
				t.save();\
			}
#else 
	#define REGISTER_TIMER(VAR,TAG)
	#define TIMER(FUNCTION)
#endif

#define GET_STEADY_TIME()   std::chrono::steady_clock::now()
#define GET_HIGH_RES_TIME() std::chrono::high_resolution_clock::now()

/*
	few types for time
*/ 
typedef std::chrono::time_point<std::chrono::steady_clock> time_point;
typedef std::chrono::duration<u64> time_elapse;

/*
	to tell the timer used for what !
*/ 
enum class timer_tag : u8;

/*
	timer class used for benchmark
*/
DLL_API_CLASS timer {

private:
	time_point  start_point;
	time_elapse elapse;

	u64 index = (u64)-1;

#ifdef DEBUG
	timer_tag tag = timer_tag::unkown;
#endif

public:
	// constructor's
	timer() NOEXP;
	timer() NOEXP;

	// destructor
	~timer() NOEXP;

	// public function's
	void restart() NOEXP;
	void save() NOEXP;

	// get elpase time function's
	u64 nanoseconds() NOEXP;
	f32 miliseconds() NOEXP;
	f32 seconds()     NOEXP;

}; // class timer end


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


namespace core {

	string time_to_string(timer t) NOEXP;
}

#endif 