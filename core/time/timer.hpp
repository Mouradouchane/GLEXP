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
	/*
		note[IMPORTANT]: #define _THIS_CODE_TIMER_TAG_ timer_tag::"your_tag", in your file before using these macro
	*/ 
	#define TIMER_THIS_SCOPE() \
			static u32 __timer__id__ = 0; \
			if(__timer__id__ == 0) __timer__id__ = timers_registery.get_id_for_timer(_THIS_CODE_TIMER_TAG_); \
			timer __t__(FUNCTION_DEFINITION , __timer__id__ , _THIS_CODE_TIMER_TAG_);

	#define TIMER_THIS_CODE(SOME_CODE) \
			{\
				static u32 __timer__id__ = 0; \
				if(__timer__id__  == 0) __timer__id__ = timers_registery.get_id_for_timer(_THIS_CODE_TIMER_TAG_); \
				timer __t__(#SOME_CODE , __timer__id__ , _THIS_CODE_TIMER_TAG_); \
				SOME_CODE; \
			}
#else 
	#define TIMER_THIS_SCOPE()
	#define TIMER_THIS_CODE(SOME_CODE) SOME_CODE ;
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
	timer_tag  _tag_ = timer_tag::unkown;
	u32        id  = (u32)-1;
	string     name;

public:
	// constructor's
	timer(string const& timer_name, u32 timer_id, timer_tag timer_tag_) NOEXP;

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


namespace timers_registery {

	u32 get_id_for_timer(timer_tag category) NOEXP;

	bool save_timer(timer const& t) NOEXP;
	timer get_timer(timer_tag category, u32 id) NOEXP;

	timer* timers(timer_tag category) NOEXP;
};

namespace core {

	string time_to_string(timer const& t) NOEXP;
}

#endif 