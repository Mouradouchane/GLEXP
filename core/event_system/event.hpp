#pragma once 

#ifndef CORE_EVENTS_HPP
#define CORE_EVENTS_HPP

/*
	--------------------------------------------------------------------------------
		event-system : used by engine parts/systems to depened on each others to do some work,
		               this event-system is multi-threaded 
		
		goal : handling "big-3" ???
			   - "Immediate Dispatch" 
			   - "Deferred Queuing"
			   - "Member-Function Binding"
	--------------------------------------------------------------------------------
*/

#include <memory>
#include <typeinfo>
#include <functional>

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/types.hpp"

#include "event_enums.hpp"

// few macros for external instantiation
#define INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(TYPE) \
		extern template event_handle start_listen<TYPE>(std::function<void(core::event<TYPE> const& _event_)> const&) noexcept; \
		extern template bool stop_listen<TYPE>(event_handle) noexcept ; \
		extern template void trigger_event<TYPE>(core::event<TYPE> const& _event_) noexcept; \
		extern template void queue_event<TYPE>(core::event<TYPE> _event_) noexcept;

#define MAP_DATATYPE_TO_EVENT(EVENT_TYPE , DATA_TYPE) \
		template<> struct event_data_type<EVENT_TYPE> { using data_type = DATA_TYPE; };

/*
	note: - each event object will carry "data-object" in it .
	      - this all of them "for now" !
*/ 

struct unkown_data {
	void* data;
	u32   size;
};

struct mouse_data {
	f32 x;
	f32 y;
	i32 scroll;
};

struct keyboard_data {
	char value; 
	i16  key; 
	bool is_special;
};

struct window_data {
	f32 x;
	f32 y;
	u16 width;
	u16 height;
};


/*
	note: this few templates just used for meta-programming to do "polymorphism" in compile-time 
*/ 
template<core::event_type etype> struct event_data_type {
	using data_type = void*;
};

// unkown-event mapping
MAP_DATATYPE_TO_EVENT(core::event_type::unkown, unkown_data);

// mouse-events mapping
MAP_DATATYPE_TO_EVENT(core::event_type::mouse_left_click,  mouse_data);
MAP_DATATYPE_TO_EVENT(core::event_type::mouse_right_click, mouse_data);
MAP_DATATYPE_TO_EVENT(core::event_type::mouse_scroll,      mouse_data);

// keyboard-events mapping
MAP_DATATYPE_TO_EVENT(core::event_type::keypress_up,   keyboard_data);
MAP_DATATYPE_TO_EVENT(core::event_type::keypress_down, keyboard_data);

// window-events mapping
MAP_DATATYPE_TO_EVENT(core::event_type::window_open,   window_data);
MAP_DATATYPE_TO_EVENT(core::event_type::window_close,  window_data);
MAP_DATATYPE_TO_EVENT(core::event_type::window_resize, window_data);

	
namespace core {

	template<core::event_type etype> class event {
		private:
			bool handled_  = false;
			bool repeated_ = false;

		public:
			COMPILE_TIME_ASSERT(std::is_void_v<typename event_data_type<etype>::data_type>, "COMPILE-TIME-ERROR: CORE -> 'unsupported' core::event_type !");
			using data_type = typename event_data_type<etype>::data_type;

			const core::event_type type = etype;
			data_type data;

			event(data_type const& event_data, bool is_repeated = false) noexcept 
				: data(event_data) , repeated_(is_repeated)
			{ }

			~event() noexcept = default;

			// event functions
			inline void set_handled()       noexcept { this->handled_ = true; }

			inline bool is_handled()        noexcept { return this->handled_; }
			inline bool is_repeated_event() noexcept { return this->repeated_; }

			// event static functions 
			static DLL_API core::event_category category() noexcept;
			static DLL_API string category_to_string(core::event_category _category_) noexcept;
			static DLL_API string type_to_string() noexcept;
	
	}; 
	// class event end


	// for unkown/custom events
	typedef core::event<core::event_type::unkown> unkown_event;

	namespace mouse_event {
		typedef core::event<core::event_type::mouse_left_click>  left_click;
		typedef core::event<core::event_type::mouse_right_click> right_click;
		typedef core::event<core::event_type::mouse_scroll>      mouse_scroll;
	}

	namespace keyboard_event {
		typedef core::event<core::event_type::keypress_up>   keypress_up;
		typedef core::event<core::event_type::keypress_down> keypress_down;
	}

	namespace window_event {
		typedef core::event<core::event_type::window_open>   window_open;
		typedef core::event<core::event_type::window_resize> window_resize;
		typedef core::event<core::event_type::window_close>  window_close;
	}

	// todo: add more namespaces for other events !!!



	// void function( core::event<core::event_type> const& _event_ );  
	#define TYPEDEF_EVENT_LISTENER(EVENT_TYPE , TYPE_NAME) \
			typedef std::function<void(core::event<EVENT_TYPE> const& _event_)> TYPE_NAME;

	namespace event_listener {
		// unkown/custom
		TYPEDEF_EVENT_LISTENER(core::event_type::unkown, unkown_event);

		// mouse 
		TYPEDEF_EVENT_LISTENER(core::event_type::mouse_left_click,  mouse_left_click);
		TYPEDEF_EVENT_LISTENER(core::event_type::mouse_right_click, mouse_right_click);
		TYPEDEF_EVENT_LISTENER(core::event_type::mouse_scroll,      mouse_scroll);
		
		// keyboard 
		TYPEDEF_EVENT_LISTENER(core::event_type::keypress_up,   keypress_up);
		TYPEDEF_EVENT_LISTENER(core::event_type::keypress_down, keypress_down);

		// window
		TYPEDEF_EVENT_LISTENER(core::event_type::window_open,   window_open);
		TYPEDEF_EVENT_LISTENER(core::event_type::window_close,  window_close);
		TYPEDEF_EVENT_LISTENER(core::event_type::window_resize, window_resize);
	}
	// namespace event_listener end


	/*
		event-system -> dispatcher
	*/ 
	namespace event_system { 

		DLL_API void init() noexcept;

		template<core::event_type etype>
		DLL_API event_handle start_listen(std::function<void(core::event<etype> const& _event_)> const& callback_function) noexcept;

		template<core::event_type etype> 
		DLL_API bool stop_listen(event_handle handle) noexcept;

		/*
			note: - "trigger_event" will block "main-thread" from exection !
		          - for none-blocking use "queue_event" !
		*/
		template<core::event_type etype> 
		DLL_API void trigger_event(core::event<etype> const& _event_) noexcept;

		template<core::event_type etype> 
		DLL_API void queue_event(core::event<etype> _event_) noexcept;


		/*
			just a way to force instantiate event_system functions for our events
		*/
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::unkown);
		// window events
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::window_open);
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::window_resize);
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::window_close);
		// mouse events
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::mouse_left_click);
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::mouse_right_click);
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::mouse_scroll);
		// keyboard events
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::keypress_up);
		INSTANTIATE_EVENT_SYSTEM_FUNCTIONS_FOR(core::event_type::keypress_down);

		// todo: instantiate the rest !!!
		
		// ==================================================================

	} // event_system end

} // namespace core end 

#include "event_impl.hpp"

#endif