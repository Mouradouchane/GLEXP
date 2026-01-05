#pragma once 

#ifndef EVENTS_CPP
#define EVENTS_CPP

#include "event.hpp"

#define EVENT_DEFAULT_PARAMETERS is_repeated
#define ASSAIGN_EVENT_DEFAULT_PARAMETERS   category_(category__), type_(type__), repeated_(is_repeated)
#define EVENT_DEFAULT_PARAMETERS_NO_VALUES bool is_repeated

namespace core{
	
	// event constructor
	event::event(
		core::event_category category__ , core::event_type type__ , EVENT_DEFAULT_PARAMETERS_NO_VALUES
	) : ASSAIGN_EVENT_DEFAULT_PARAMETERS {

	};

	// event public functions
	inline void event::set_handled() noexcept { this->handled_ = true; }
	inline bool event::is_handled() noexcept { return this->handled_; }
	inline bool event::is_repeated_event() noexcept { return this->repeated_; }

	namespace mouse_event {

		mouse_event::mouse_click_left::mouse_click_left(i32 x, i32 y, EVENT_DEFAULT_PARAMETERS_NO_VALUES) 
			: x_(x) , y_(y) , core::event(core::event_category::mouse , core::event_type::mouse_click_left , EVENT_DEFAULT_PARAMETERS)
		{
			
		}

		mouse_event::mouse_click_right::mouse_click_right(i32 x, i32 y, EVENT_DEFAULT_PARAMETERS_NO_VALUES) 
			: x_(x) , y_(y) , core::event(core::event_category::mouse , core::event_type::mouse_click_right , EVENT_DEFAULT_PARAMETERS)
		{

		}

	}
	

} // namespace core end

#endif