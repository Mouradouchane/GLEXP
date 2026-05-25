#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_HPP
#define CORE_KEYBOARD_EVENTS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/events/event_object.hpp"
#include "core/events/event_dispatcher.hpp"
#include "core/events/event_manager.hpp"

#include "core/window/window.hpp"

namespace core {

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

	// key events types
	struct key_up		    : core::event<key_data>  { };
	struct key_down			: core::event<key_data>  { };
	struct special_key_up	: core::event<key_data>  { };
	struct special_key_down : core::event<key_data>  { };
	struct keys_down		: core::event<keys_data> { };
	

	/*
		keyboard events - manager
	*/ 
	DLL_API_CLASS keyboard {
	private:
		core::event_manager    manager;
		window_internal_object window;

	public:

		// events list
		enum class event_type : u16 {
			key_up,
			key_down,
			keys_down,
			special_key_up,
			special_key_down, 
		};

		// constructor
		keyboard(
			core::window const& target_window, 
			STRING name, 
			core::memory_allocator const& allocator,
			u32 size = 32, 
			u32 resize_value = 32
		);

		// destructor
		~keyboard();

		// public functions
		void poll_events() NOEXP;

		listener_id start_listen(core::callback<core::key_up>			 const& callback_function) NOEXP;
		listener_id start_listen(core::callback<core::key_down>			 const& callback_function) NOEXP;
		listener_id start_listen(core::callback<core::keys_down>	     const& callback_function) NOEXP;
		listener_id start_listen(core::callback<core::special_key_up>	 const& callback_function) NOEXP;
		listener_id start_listen(core::callback<core::special_key_down>	 const& callback_function) NOEXP;

		bool stop_listen(listener_id event_listener_id) NOEXP;

	}; // class keyboard end

} // namespace core end

#endif