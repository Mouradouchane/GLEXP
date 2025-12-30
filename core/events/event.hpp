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

	namespace event_system {

		// event's 
		enum class event_type : u16 {
			none = 0,
			custom,

			mouse,
			keyboard,
			controller,

			gui,
			gpu,
			screen,

			graphics,
			physics,
			animation,
			network,

			file,
			memory,
		};

		struct event {
			event_type type;

			bool       is_immdiate;
		};

		// event listening
		DLL_API event_id start_listen(event_type);
		DLL_API bool     stop_listen();

		// trigger events
		DLL_API void trigger_event(event_type);
		DLL_API void queue_event();


	} // namespace event 

} // namespace core

#endif