#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_CPP 
#define CORE_KEYBOARD_EVENTS_CPP

#include "core/macros.hpp"
#include "keyboard.hpp"

#ifdef DEBUG
	static auto _key_event_logger_ = CORE_GET_LOGGER(EVENT_SYSTEM_LOGGER);
#else 
	static auto _key_event_logger_ = nullptr;
#endif

#define _LOGGER_ _key_event_logger_

namespace core {

	namespace keyboard {
		
		core::event_manager manager(core::event_manager_category::keyboard_events , "KEYBOARD_EVENTS_MANAGER");

		DLL_API listener_id start_listen(core::callback<key_up> const& callback_function) {
			return keyboard::manager.start_listen<key_up>(callback_function);
		}

		DLL_API listener_id start_listen(core::callback<key_down> const& callback_function){
			return keyboard::manager.start_listen<key_down>(callback_function);
		}

		DLL_API listener_id start_listen(core::callback<special_key_up> const& callback_function){
			return keyboard::manager.start_listen<special_key_up>(callback_function);
		}

		DLL_API listener_id start_listen(core::callback<special_key_down> const& callback_function){
			return keyboard::manager.start_listen<special_key_down>(callback_function);
		}

		DLL_API listener_id start_listen(core::callback<keys_down> const& callback_function){
			return keyboard::manager.start_listen<keys_down>(callback_function);
		}

		DLL_API bool stop_listen(listener_id event_listener_id) {
			return keyboard::manager.stop_listen(event_listener_id);
		}

	} // namespace keyboard end

} // namespace core end

#endif 