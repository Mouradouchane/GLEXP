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
	#define TIMER_THIS() timer __t__(FUNCTION_DEFINITION , _TIMER_TAG_);

	#define TIMER(SOME_CODE , TIMER_TAG) \
			{\
				timer __t__(FUNCTION_DEFINITION , TIMER_TAG); \
				SOME_CODE; \
			}
#else 
	#define TIMER_THIS()
	#define TIMER(SOME_CODE , TIMER_TAG) SOME_CODE
#endif

#define GET_STEADY_TIME()   std::chrono::steady_clock::now()
#define GET_HIGH_RES_TIME() std::chrono::high_resolution_clock::now()

#define GET_ELAPSE_TIME(START, END)     std::chrono::duration_cast<time_elapse>(END - START)
#define GET_ELAPSE_TIME_NS(START, END)  std::chrono::duration_cast<time_ns>(END - START)
#define GET_ELAPSE_TIME_MS(START, END)  std::chrono::duration_cast<time_ms>(END - START)
#define GET_ELAPSE_TIME_SEC(START, END) std::chrono::duration_cast<time_sec>(END - START)

/*
	few types for time
*/ 
typedef std::chrono::time_point<std::chrono::steady_clock>            time_point;
typedef std::chrono::duration<u64, std::chrono::steady_clock::period> time_elapse;

typedef std::chrono::duration<u64, std::nano>     time_ns;
typedef std::chrono::duration<u32, std::milli>    time_ms;
typedef std::chrono::duration<u32, std::ratio<1>> time_sec;


/*
	timer tag to tell what timer used for !
	helps in performance benchmark and debugging
*/ 
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


/*
	timer class used for benchmark
*/
DLL_API_CLASS timer {

private:
	time_point start_point;

	u64 index = (u64)-1;

#ifdef DEBUG
	string    name;
	timer_tag tag = timer_tag::unkown;
#endif

public:
	// constructor's
	timer(string const& name, timer_tag tag) NOEXP;

	// destructor
	~timer() NOEXP;

	// public function's
	void restart(bool save_result = false) NOEXP;
	void save() NOEXP;

	// get elpase time function's
	u64 nanoseconds() NOEXP;
	u32 miliseconds() NOEXP;
	u32 seconds()     NOEXP;

}; // class timer end


namespace core {

	string time_to_string(timer const& t) NOEXP;
}

#endif 