#if 0
#pragma once

#ifndef CORE_TIMER_CPP
#define CORE_TIMER_CPP

#include "core/logger/logger.hpp"
#include "core/memory/memory.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"
#include "core/containers/arrays/array.hpp"

#include "timer.hpp"

#ifdef DEBUG
	static auto _core_timer_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_timer_logger_ = nullptr;
#endif

#define _LOGGER_  _core_timer_logger_ 

// memory allocator for timers registers
core::dynamic_allocator timers_allocator( 
	core::dynamic_allocator_configs{ 
		.name  = string("timers_allocator"),
		._tag_ = subsystem_memory_tag::time_system,
		.memory_budget = 16 MB ,
		.max_allocations_per_block = 100,
		.is_multi_thread = false,
		.allocate_all_at_once = true
	} 
);

#define TIMERS_REGISTERS_COUNT 255
core::array<core::array<timer>> timers_registery(TIMERS_REGISTERS_COUNT , &timers_allocator , memory_tag::timer);

/*
	constructor
*/
timer::timer(string const& timer_name, u32 timer_id, timer_tag timer_tag_) NOEXP {
	this->_tag_  = timer_tag_;
	this->_id_   = timer_id;
	this->_name_ = timer_name;

	this->start_point = GET_STEADY_TIME();
}

/*
	destructor
*/
timer::~timer() NOEXP {
	time_point end_point = GET_STEADY_TIME();
	time_elapse   elapse = GET_ELAPSE_TIME(this->start_point , end_point);

	// todo: save the result in timer's registery

	CORE_TRACE("{} : {}", elapse, this->_name_);
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


// todo:
string core::time_to_string(timer const& t) NOEXP {
	return string("todo:implement this !");
}

#endif
#endif 