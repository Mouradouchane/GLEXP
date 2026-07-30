#if 1
#pragma once

#ifndef CORE_TIMER_CPP
#define CORE_TIMER_CPP

#include "timer.hpp"
#include "core/logger/logger.hpp"

#ifdef DEBUG
	static auto _core_timer_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_timer_logger_ = nullptr;
#endif

#define _LOGGER_  _core_timer_logger_ 

/*
	constructor
*/
timer::timer(string const& name, timer_tag tag) NOEXP {
#ifdef DEBUG
	this->name = name;
	this->tag  = tag;
#endif

	this->start_point = GET_STEADY_TIME();
}

/*
	destructor
*/
timer::~timer() NOEXP {
	time_point end_point = GET_STEADY_TIME();
	time_elapse   elapse = GET_ELAPSE_TIME(this->start_point , end_point);

	// todo: save the result in timer's registery

	CORE_TRACE("{}:{}", elapse, this->name);
}

/*
	timer function's
*/

void timer::restart(bool save_result) NOEXP {
	if (save_result) this->save();
	this->start_point = GET_STEADY_TIME();
}

void timer::save() NOEXP {
	// todo: implement this
}

u64 timer::nanoseconds() NOEXP{
	time_point current_point = GET_STEADY_TIME();
	time_ns elapse = GET_ELAPSE_TIME_NS(this->start_point, current_point);

	return elapse.count();
}

u32 timer::miliseconds() NOEXP{
	time_point current_point = GET_STEADY_TIME();
	time_ms    elapse = GET_ELAPSE_TIME_MS(this->start_point, current_point);

	return elapse.count();
}

u32 timer::seconds() NOEXP{
	time_point current_point = GET_STEADY_TIME();
	time_sec   elapse = GET_ELAPSE_TIME_SEC(this->start_point, current_point);

	return elapse.count();
}


string core::time_to_string(timer const& t) NOEXP {

	
}

#endif
#endif 