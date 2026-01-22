#pragma once 

#ifndef EVENTS_HPP
#define EVENTS_HPP

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
#include <functional>

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"

namespace core {

	enum class event_category : u16 {
			unkown = 0,

			mouse,
			keyboard,
			controller,

			window,
			gui,
			graphics,
			physics,
			animation,
			network,

		// for "debug only" !!!
		#ifdef DEBUG
			file,
			memory,
		#endif

	}; // event category end

	enum class event_type : u16 {
			unkown = 0,

			// window events
			window_open,
			window_resize,
			window_close,

			// mouse events
			mouse_click_left,
			mouse_click_right,
			mouse_scroll,

			// keyboard events
			keypress_up,
			keypress_down,

		// for "debug only" !!!
		#ifdef DEBUG
			// files events
			file_open,
			file_close,
			file_write,
			file_read,
			file_exist,
			file_not_exist,

			// memory events
			memory_allocated,
			memory_deallocated,
		#endif

	}; // event_type end


	/*
		=========== events data objects ====================================================
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
	// ========================================================================================
	

	// ========================================================================================
	// note: "event_data_type" just used to do type mapping in "class event" !
	//       basically the type of the data in event gonna be determineted at compile-time 
	//       based on "event_type"
	// ========================================================================================
	template<core::event_type event__type> struct event_data_type {
		using data_type = void*;
	};
	
	// unkown-event mapping
	template<> struct event_data_type<event_type::unkown>            { using data_type = unkown_data; };

	// mouse-events mapping
	template<> struct event_data_type<event_type::mouse_click_left>  { using data_type = mouse_data; };
	template<> struct event_data_type<event_type::mouse_click_right> { using data_type = mouse_data; };
	template<> struct event_data_type<event_type::mouse_scroll>      { using data_type = mouse_data; };

	// keyboard-events mapping
	template<> struct event_data_type<event_type::keypress_up>       { using data_type = keyboard_data; };
	template<> struct event_data_type<event_type::keypress_down>     { using data_type = keyboard_data; };

	// window-events mapping
	template<> struct event_data_type<event_type::window_open>       { using data_type = window_data; };
	template<> struct event_data_type<event_type::window_close>      { using data_type = window_data; };
	template<> struct event_data_type<event_type::window_resize>     { using data_type = window_data; };

	// ========================================================================================
	 
	/*
		main event class
	*/
	template<core::event_type event__type> class event {

	private:
		bool handled_  = false;
		bool repeated_ = false;

	public:
		COMPILE_TIME_ASSERT(std::is_void_v<typename event_data_type<event__type>::data_type>, "COMPILE-TIME-ERROR: CORE -> 'unsupported' event type !");
		using data_type = typename event_data_type<event__type>::data_type;

		const core::event_type type = event__type;
		data_type data;

		event(data_type const& event_data, bool is_repeated = false) noexcept 
			: data(event_data) , repeated_(is_repeated)
		{

		}

		~event() noexcept = default;

		// event functions
		inline void set_handled()       noexcept { this->handled_ = true; }

		inline bool is_handled()        noexcept { return this->handled_; }
		inline bool is_repeated_event() noexcept { return this->repeated_; }

		// event static functions 
		static core::event_category category(event const& event_object) noexcept;
		static string category_string(event const& event_object) noexcept;
		static string type_string(event const& event_object) noexcept;

	}; 
	// class event end

	typedef core::event<core::event_type::unkown> unkown_event;

	/*
		=============== mouse events ===============
	*/
	namespace mouse_event {
		typedef core::event<core::event_type::mouse_click_left>  left_click;
		typedef core::event<core::event_type::mouse_click_right> right_click;
		typedef core::event<core::event_type::mouse_scroll>      scroll;
	}

	/*
		=============== keyboard events ===============
	*/
	namespace keyboard_event {
		typedef core::event<core::event_type::keypress_up>   key_up;
		typedef core::event<core::event_type::keypress_down> key_down;
	}

	/*
		=============== window events =================
	*/
	namespace window_event {
		typedef core::event<core::event_type::window_open>   window_open;
		typedef core::event<core::event_type::window_resize> window_resize;
		typedef core::event<core::event_type::window_close>  window_close;
	}
	
	// ===========================================================================


	/*
		note: - callback function used for event listening .
		      - void function( core::event<event_type> const& event );  
	*/ 
	template<core::event_type event__type>     
	using event_listener_function = std::function<void(core::event<event__type> const& _event_)>;


	// event's manager 
	namespace dispatcher { 

		template<core::event_type event__type>
		event_listener_id start_listening(event_listener_function<event__type> listener_function) noexcept;

		template<core::event_type event__type> 
		void stop_listening(event_listener_id id) noexcept;

		/*
			note: - "trigger_event" will block "main-thread" from exection !
		          - for none-blocking use "queue_event" !
		*/
		template<core::event_type event__type> 
		void trigger_event(core::event<event__type> const& _event_) noexcept;

		template<core::event_type event__type> 
		void queue_event(std::unique_ptr<core::event<event__type>> _event_) noexcept;

	} // namespace dispatcher end


} // namespace core end


/*
	 template's implemenations
*/

namespace core {

	// event's manager 
	namespace dispatcher { 

		template<core::event_type event__type>
		event_listener_id start_listening(event_listener_function<event__type> listener_function) noexcept {
			// todo: implement this !
		}

		template<core::event_type event__type>
		void stop_listening(event_listener_id id) noexcept {
			// todo: implement this !
		}

		template<core::event_type event__type> 
		void trigger_event(core::event<event__type> const& _event_) noexcept {
			// todo: implement this !
		}

		template<core::event_type event__type> 
		void queue_event(std::unique_ptr<core::event<event__type>> _event_) noexcept {
			// todo: implement this !
		}

	} // namespace dispatcher end

} // namespace core end 


// event static functions 
template<core::event_type event__type> static core::event_category core::event<event__type>::category(event const& event_object) noexcept {

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

template<core::event_type event__type> static string core::event<event__type>::type_string(event const& event_object) noexcept {

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

template<core::event_type event__type> static string core::event<event__type>::category_string(event const& event_object) noexcept {
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