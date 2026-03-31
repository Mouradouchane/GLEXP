#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_HPP
#define CORE_KEYBOARD_EVENTS_HPP

#include "core/types.hpp"
#include "core/events/dispatcher/dispatcher.hpp"
#include "core/events/manager/manager.hpp"
#include "core/events/event.hpp"

namespace core {
	
	struct keyboard_data {
		char value; 
		i16  key; 
		bool is_special;
	};

	class keyboard {
	private:
		static const core::event_manager _manager_(
			core::events_category::keyboard_events , "core_main_event_manager_keyboard"
		);

	public:
		// keyboard types of events
		enum class event_type : u16 {
			// normal keys
			keypress_up = 1,
			keypress_down,
			// special keys
			keypress_up_special,
			keypress_down_special,
		};

		DLL_API listener_id start_listen(core::callback<core::event<event_type::keypress_down>> const& callback_function);
		DLL_API bool stop_listen(listener_id id);

	}; // class keyboard_event end


} // namespace core end

#endif