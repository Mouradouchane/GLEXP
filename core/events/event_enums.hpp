#pragma once 

#ifndef EVENT_ENUMS_HPP
#define EVENT_ENUMS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

namespace core {

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

		file,
		memory,

	}; 
	// enum event-category end


	enum class event_type : u16 {
		unkown = 0,

		// window events
		window_open,
		window_resize,
		window_close,

		// mouse events
		mouse_left_click,
		mouse_right_click,
		mouse_scroll,

		// keyboard events
		keypress_up,
		keypress_down,

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

	}; // enum event-type end

} // namespace core end

#endif