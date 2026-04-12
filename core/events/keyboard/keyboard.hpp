#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_HPP
#define CORE_KEYBOARD_EVENTS_HPP

#include "core/types.hpp"
#include "core/events/event_dispatcher.hpp"
#include "core/events/event_manager.hpp"

namespace core {
	
	namespace keyboard {

		// event object
		struct event {
			char value; // key value
			i16  key;   // physical key 
			bool is_special;

		#ifdef DEBUG
			u16 type;
			u16 category;
		#endif
		};

		// keyboard events
		enum class event_type : u16 {
			key_press_up = 1,
			key_press_down,
			special_key_press_down,
			multi_key_press_down,
		};

		DLL_API listener_id on_event(
			core::keyboard::event_type keyboard_event, 
			core::callback<core::keyboard::event> const& callback_function
		);

		DLL_API bool stop(listener_id event_listener_id);

	}; // namespace keyboard end

} // namespace core end

#endif