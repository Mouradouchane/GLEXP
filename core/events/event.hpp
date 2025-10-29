#pragma once 

#ifndef EVENTS_HPP
#define EVENTS_HPP

/*
	--------------------------------------------------------------------------------
		event system :
		used to create events , trigger event , listen to events
	--------------------------------------------------------------------------------
*/

#include "core/macros.hpp"
#include "core/types.hpp"

namespace core {

	namespace event {

		// events list
		enum class list : u16 {
			none = 0,
			mouse_left_click,
			mouse_right_click,

		};

		// add/remove events
		DLL_API bool create_event();
		DLL_API bool remove_event();

		// add/remove event listeners
		DLL_API event_id start_listen();
		DLL_API bool     stop_listen();

		// trigger events
		DLL_API void trigger_event();
		DLL_API void queue_event();


	} // namespace event 

} // namespace core

#endif