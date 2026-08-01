#if 1
#pragma once

#ifndef CORE_TIMER_CPP
#define CORE_TIMER_CPP

#include <unordered_map>
#include <array>

#include "timer.hpp"
#include "core/logger/logger.hpp"

#ifdef DEBUG
	static auto _core_timer_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_timer_logger_ = nullptr;
#endif

#define _LOGGER_  _core_timer_logger_ 

#define MAX_TIMERS_REGISTERS 255
std::array<std::unordered_map<u32, timer>, MAX_TIMERS_REGISTERS> timers_registery();

/*
	constructor
*/
timer::timer(string const& timer_name, u32 timer_id, timer_tag timer_tag_) NOEXP {
	this->tag  = timer_tag_;
	this->id   = timer_id;
	this->name = timer_name;

	this->start_point = GET_STEADY_TIME();
}

/*
	destructor
*/
timer::~timer() NOEXP {
	time_point end_point = GET_STEADY_TIME();
	time_elapse   elapse = GET_ELAPSE_TIME(this->start_point , end_point);

	// todo: save the result in timer's registery

	CORE_TRACE("{} : {}", elapse, this->name);
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