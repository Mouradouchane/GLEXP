#pragma once 

#ifndef EVENT_IMPL_HPP
#define EVENT_IMPL_HPP

/*
	note: this used only by event.hpp , here implementation only .
*/

template<event_type etype> 
static event_category core::event<etype>::category(event const& event_object) noexcept {

	switch (event_object.type) {
		// mouse
	case event_type::mouse_scroll      : return event_category::mouse; break;
	case event_type::mouse_click_left  : return event_category::mouse; break;
	case event_type::mouse_click_right : return event_category::mouse; break;

		// keyboard
	case event_type::keypress_down : return event_category::keyboard; break;
	case event_type::keypress_up   : return event_category::keyboard; break;

		// window
	case event_type::window_open   : return event_category::window; break;
	case event_type::window_resize : return event_category::window; break;
	case event_type::window_close  : return event_category::window; break;

		// todo : implement more event categories
		// controller
		// gui
		// graphics
		// physics
		// animation
		// network

		// for "debug only" !!!
#ifdef DEBUG
		// files
	case event_type::file_open      : return event_category::file; break;
	case event_type::file_close     : return event_category::file; break;
	case event_type::file_write	    : return event_category::file; break;
	case event_type::file_read	    : return event_category::file; break;
	case event_type::file_exist	    : return event_category::file; break;
	case event_type::file_not_exist	: return event_category::file; break;

		// memory events
	case event_type::memory_allocated   : return event_category::memory; break;
	case event_type::memory_deallocated : return event_category::memory; break;
#endif

		default return event_category::unkown;
	}

}

template<event_type etype> 
static string core::event<etype>::type_to_string(event const& event_object) noexcept {

	switch (event_object.type) {
		// mouse
	case event_type::mouse_scroll      : return "mouse_scroll";      break;
	case event_type::mouse_click_left  : return "mouse_click_left";  break;
	case event_type::mouse_click_right : return "mouse_click_right"; break;

		// keyboard
	case event_type::keypress_up   : return "keypress_up";   break;
	case event_type::keypress_down : return "keypress_down"; break;

		// window
	case event_type::window_open   : return "window_open";   break;
	case event_type::window_resize : return "window_resize"; break;
	case event_type::window_close  : return "window_close";  break;

		// todo: implement more event categories
		// controller
		// gui
		// graphics
		// physics
		// animation
		// network

		// for "debug only" !!!
#ifdef DEBUG
		// files
	case event_type::file_open      : return "file_open";      break;
	case event_type::file_close     : return "file_close";     break;
	case event_type::file_write	    : return "file_write";     break;
	case event_type::file_read	    : return "file_read";      break;
	case event_type::file_exist	    : return "file_exist";     break;
	case event_type::file_not_exist	: return "file_not_exist"; break;

		// memory events
	case event_type::memory_allocated   : return "memory_allocated";   break;
	case event_type::memory_deallocated : return "memory_deallocated"; break;
#endif

		default return "unkown";
	}

}

template<event_type etype> 
static string core::event<etype>::category_to_string(event const& event_object) noexcept {
	event_category category = core::event::category(event_object);

	switch (category) {
	case event_category::mouse      : return "mouse";      break;
	case event_category::keyboard   : return "keyboard";   break;
	case event_category::controller : return "controller"; break;
	case event_category::window	    : return "window";     break;
	case event_category::gui	    : return "gui";        break;
	case event_category::graphics	: return "graphics";   break;
	case event_category::physics	: return "physics";    break;
	case event_category::animation	: return "animation";  break;
	case event_category::network	: return "network";    break;

		// for "debug only" !!!
#ifdef DEBUG
	case event_category::file   : return "file";   break;
	case event_category::memory : return "memory"; break;
#endif

		default return "unkown";
	}

}

#endif