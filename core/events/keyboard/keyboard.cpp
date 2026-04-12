#pragma once 

#ifndef CORE_KEYBOARD_EVENTS_CPP 
#define CORE_KEYBOARD_EVENTS_CPP

#include "core/macros.hpp"
#include "keyboard.hpp"

#ifdef DEBUG
	static auto _logger_ = CORE_GET_LOGGER(EVENT_SYSTEM_LOGGER);
#endif

#define FORCE_INSTANTIATE_START_LISTEN(EVENT_ENUM) \
		template DLL_API listener_id start_listen<EVENT_ENUM>(core::callback<core::event<key_data>> const& _callback_);

namespace core {

	namespace keyboard {
		
		core::event_manager manager(core::event_manager_category::keyboard_events , "KEYBOARD_EVENTS_MANAGER");

		template<core::keyboard::event_type etype>
		listener_id start_listen(core::callback<core::event<key_data>> const& callback_function) {

			u16 index = S_CAST(etype, u16);
			
		}

		FORCE_INSTANTIATE_START_LISTEN(event_type::key_up);
		FORCE_INSTANTIATE_START_LISTEN(event_type::key_down);
		FORCE_INSTANTIATE_START_LISTEN(event_type::special_key_down);


	} // namespace keyboard end

} // namespace core end

#endif 