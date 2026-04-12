#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_HPP
#define CORE_KEYBOARD_EVENTS_HPP

#include "core/types.hpp"
#include "core/events/event_object.hpp"
#include "core/events/event_dispatcher.hpp"
#include "core/events/event_manager.hpp"

// macro to just tell the compiler that start_listen function
// implmentation is external
#define EXTERN_START_LISTEN(EVENT_ENUM) \
		extern template DLL_API listener_id start_listen<EVENT_ENUM>(core::callback<core::event<key_data>> const& _callback_);

namespace core {
	
	namespace keyboard {

		// keyboard event objects
		struct key_data {
			char value; // key value
			i16  key;   // physical key 
			bool is_special;
		};

		struct keys_data {
			key_data* keys;
			u32 size;
		};

		struct key_up		    : core::event<key_data> { };
		struct key_down			: core::event<key_data> { };
		struct special_key_down	: core::event<key_data> { };
		struct keys_down		: core::event<keys_data> { };

		// keyboard events types
		enum class event_type : u16 {
			unkown = 0,
			key_up,
			key_down,
			keys_down,
			special_key_up,
			special_key_down, 
		};


		template<core::keyboard::event_type etype>
		listener_id start_listen( core::callback<core::event<key_data>> const& callback_function );

		DLL_API listener_id start_listen( core::callback<keys_down> const& callback_function );

		DLL_API bool stop_listen(listener_id event_listener_id);

		EXTERN_START_LISTEN(event_type::key_up);
		EXTERN_START_LISTEN(event_type::key_down);
		EXTERN_START_LISTEN(event_type::special_key_down);

	}; // namespace keyboard end

} // namespace core end

#endif