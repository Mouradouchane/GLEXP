#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_HPP
#define CORE_KEYBOARD_EVENTS_HPP

#include "core/types.hpp"
#include "core/events/event_object.hpp"
#include "core/events/event_dispatcher.hpp"
#include "core/events/event_manager.hpp"

namespace core {
	
	namespace keyboard {

		// event objects
		struct key_data {
			char value; // key value
			i16  key;   // physical key 
			bool is_special;
		};

		struct keys_data {
			key_data* keys;
			u32 size;
		};


		// types
		struct key_up		    : core::event<key_data> { };
		struct key_down			: core::event<key_data> { };
		struct special_key_up	: core::event<key_data> { };
		struct special_key_down : core::event<key_data> { };

		struct keys_down		: core::event<keys_data> { };

		// events list
		enum class event_type : u16 {
			key_up,
			key_down,
			keys_down,
			special_key_up,
			special_key_down, 
		};

		// keyboard event-manager functions

		DLL_API listener_id start_listen(core::callback<key_up>				const& callback_function);
		DLL_API listener_id start_listen(core::callback<key_down>			const& callback_function);
		DLL_API listener_id start_listen(core::callback<special_key_up>		const& callback_function);
		DLL_API listener_id start_listen(core::callback<special_key_down>	const& callback_function);
		DLL_API listener_id start_listen(core::callback<keys_down>			const& callback_function);

		DLL_API bool stop_listen(listener_id event_listener_id);

	}; // namespace keyboard end

} // namespace core end

#endif