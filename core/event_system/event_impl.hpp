#pragma once 

#ifndef CORE_EVENT_IMPL_HPP
#define CORE_EVENT_IMPL_HPP

/*
	note: this used only by event.hpp , just implementation here .
*/


template<core::event_type etype> 
static core::event_category core::event<etype>::category(event const& event_object) noexcept {

	switch (event_object.type) {
		// mouse
		case core::event_type::mouse_scroll      : return event_category::mouse; break;
		case core::event_type::mouse_click_left  : return event_category::mouse; break;
		case core::event_type::mouse_click_right : return event_category::mouse; break;
		// keyboard
		case core::event_type::keypress_down : return event_category::keyboard; break;
		case core::event_type::keypress_up   : return event_category::keyboard; break;
		// window
		case core::event_type::window_open   : return event_category::window; break;
		case core::event_type::window_resize : return event_category::window; break;
		case core::event_type::window_close  : return event_category::window; break;
		// files
		case core::event_type::file_open       : return event_category::file; break;
		case core::event_type::file_close      : return event_category::file; break;
		case core::event_type::file_write	   : return event_category::file; break;
		case core::event_type::file_read	   : return event_category::file; break;
		case core::event_type::file_exist	   : return event_category::file; break;
		case core::event_type::file_not_exist  : return event_category::file; break;
		// memory events
		case core::event_type::memory_allocated   : return event_category::memory; break;
		case core::event_type::memory_deallocated : return event_category::memory; break;

		// todo : implement the rest
		// controller
		// gui
		// graphics
		// physics
		// animation
		// network

		default return core::event_category::unkown;
	}

}


#define CASE_EVENT_TYPE_TO_STR(EVENT_TYPE) \
		case core::event_type:: EVENT_TYPE : return #EVENT_TYPE; break;

template<core::event_type etype> 
static string core::event<etype>::type_to_string(event const& event_object) noexcept {

	switch (event_object.type) {
		// mouse
		CASE_EVENT_TYPE_TO_STR( core::event_type::mouse_scroll      );
		CASE_EVENT_TYPE_TO_STR( core::event_type::mouse_click_left  );
		CASE_EVENT_TYPE_TO_STR( core::event_type::mouse_click_right );
		// keyboard
		CASE_EVENT_TYPE_TO_STR( core::event_type::keypress_up   );
		CASE_EVENT_TYPE_TO_STR( core::event_type::keypress_down );
		// window
		CASE_EVENT_TYPE_TO_STR( core::event_type::window_open   );
		CASE_EVENT_TYPE_TO_STR( core::event_type::window_resize );
		CASE_EVENT_TYPE_TO_STR( core::event_type::window_close  );
		// files
		CASE_EVENT_TYPE_TO_STR( core::event_type::file_open      );
		CASE_EVENT_TYPE_TO_STR( core::event_type::file_close     );
		CASE_EVENT_TYPE_TO_STR( core::event_type::file_write	 );
		CASE_EVENT_TYPE_TO_STR( core::event_type::file_read	     );
		CASE_EVENT_TYPE_TO_STR( core::event_type::file_exist	 );
		CASE_EVENT_TYPE_TO_STR( core::event_type::file_not_exist );
		// memory
		CASE_EVENT_TYPE_TO_STR( core::event_type::memory_allocated   );
		CASE_EVENT_TYPE_TO_STR( core::event_type::memory_deallocated );

		// todo: implement the rest
		// controller
		// gui
		// graphics
		// physics
		// animation
		// network

		default return "unkown";
	}

}


#define CASE_EVENT_CATEGORY_TO_STRING(EVENT_CATEGORE) \
		case core::event_category:: EVENT_CATEGORE : return #EVENT_CATEGORE; break;

template<core::event_type etype> 
static string core::event<etype>::category_to_string(event const& event_object) noexcept {
	
	core::event_category category = core::event::category(event_object);

	switch (category) {

		CASE_EVENT_CATEGORY_TO_STRING( mouse      );
		CASE_EVENT_CATEGORY_TO_STRING( keyboard   );
		CASE_EVENT_CATEGORY_TO_STRING( controller );
		CASE_EVENT_CATEGORY_TO_STRING( window	  );
		CASE_EVENT_CATEGORY_TO_STRING( gui	      );
		CASE_EVENT_CATEGORY_TO_STRING( graphics	  );
		CASE_EVENT_CATEGORY_TO_STRING( physics	  );
		CASE_EVENT_CATEGORY_TO_STRING( animation  );
		CASE_EVENT_CATEGORY_TO_STRING( network	  );
		CASE_EVENT_CATEGORY_TO_STRING( file       );
		CASE_EVENT_CATEGORY_TO_STRING( memory     );

		default return "unkown";
	}

}

#endif