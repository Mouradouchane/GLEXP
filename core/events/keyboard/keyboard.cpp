#if 0
#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_CPP 
#define CORE_KEYBOARD_EVENTS_CPP

#include "core/macros.hpp"
#include "core/assert.hpp"

#include "keyboard.hpp"

#ifdef DEBUG
	static auto _key_event_mananger_logger_ = CORE_GET_LOGGER(EVENT_SYSTEM_LOGGER);
#else
	static auto _key_event_mananger_logger_ = nullptr;
#endif

#define _LOGGER_ _key_event_mananger_logger_


core::keyboard::keyboard(
	core::window const& target_window,
	STRING name,
	core::dynamic_allocator const& allocator,
	u32 size,
	u32 resize_value
) NOEXP {

	this->window  = target_window.get_internal_object();

	this->manager = core::event_manager(
		core::event_manager_category {
			.category = core::event_manager_category::keyboard_events, 
			.name = name,
			.size = size,
			.resize = resize_value, 
			.allocator = allocator
		}
	);

}

/*
	destructor
*/
core::keyboard::~keyboard() NOEXP {
	this->manager.~event_manager();
	this->window.window = nullptr;
}

/*
	keyboard event-system public functions
*/

void core::keyboard::poll_events() NOEXP {

	/*
		todo: implement this
	*/

}

listener_id core::keyboard::start_listen(core::callback<key_up> const& callback_function) NOEXP {
	return this->manager.start_listen<key_up>(callback_function);
}

listener_id core::keyboard::start_listen(core::callback<key_down> const& callback_function) NOEXP {
	return this->manager.start_listen<key_down>(callback_function);
}

listener_id core::keyboard::start_listen(core::callback<special_key_up> const& callback_function) NOEXP {
	return this->manager.start_listen<special_key_up>(callback_function);
}

listener_id core::keyboard::start_listen(core::callback<special_key_down> const& callback_function) NOEXP {
	return this->manager.start_listen<special_key_down>(callback_function);
}

listener_id core::keyboard::start_listen(core::callback<keys_down> const& callback_function) NOEXP {
	return this->manager.start_listen<keys_down>(callback_function);
}

bool core::keyboard::stop_listen(listener_id event_listener_id) NOEXP {
	return this->manager.stop_listen(event_listener_id);
}

#endif
#endif 