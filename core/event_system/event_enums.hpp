#pragma once 

#ifndef EVENT_ENUMS_HPP
#define EVENT_ENUMS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

enum class event_category : u16 {
	unkown = 0,

	mouse,
	keyboard,
	controller,

	window,
	gui,
	graphics,
	physics,
	animation,
	network,

	// for "debug only" !!!
#ifdef DEBUG
	file,
	memory,
#endif

}; // event category end

enum class event_type : u16 {
	unkown = 0,

	// window events
	window_open,
	window_resize,
	window_close,

	// mouse events
	mouse_click_left,
	mouse_click_right,
	mouse_scroll,

	// keyboard events
	keypress_up,
	keypress_down,

	// for "debug only" !!!
#ifdef DEBUG
	// files events
	file_open,
	file_close,
	file_write,
	file_read,
	file_exist,
	file_not_exist,

	// memory events
	memory_allocated,
	memory_deallocated,
#endif

}; // event_type end


#endif