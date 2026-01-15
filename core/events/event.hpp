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
		none = 0,
		// for unlisted category
		custom,

		mouse,
		keyboard,
		controller,

		gui,
		graphics,
		physics,
		animation,
		network,

	// for "debug only" !!!
	#ifdef DEBUG
		file,
		folder,
		memory,
	#endif
	};

	enum class event_type : u16 {
		none = 0,
		// for unlisted event
		custom,

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

		// folder events
		folder_exist,
		folder_not_exist,

		// memory events
		memory_allocated,
		memory_deallocated,
	#endif

	}; // event_type end

	// =========== events data objects ===========
	struct mouse_data {
		f32 x;
		f32 y;
		i32 scroll;
	};

	struct keyboard_data {
		char key;
		bool is_special;
	};

	// ========================================================================================
	// note: "event_data_type" just used to do type mapping in "class event" !
	//       basically the type of the data in event gonna be determineted at compile-time 
	//       based on "event_type"
	// ========================================================================================
	template<core::event_type event__type> struct event_data_type {
		using data_type = void*;
	};

	// mouse-events mapping
	template<> struct event_data_type<event_type::mouse_click_left>   { using data_type = mouse_data; };
	template<> struct event_data_type<event_type::mouse_click_right>  { using data_type = mouse_data; };
	template<> struct event_data_type<event_type::mouse_scroll>       { using data_type = mouse_data; };

	// keyboard-events mapping
	template<> struct event_data_type<event_type::keypress_up>       { using data_type = keyboard_data; };
	template<> struct event_data_type<event_type::keypress_down>     { using data_type = keyboard_data; };

	// ========================================================================================
	 

	template<core::event_type event__type> class event {

	private:
		bool handled_  = false;
		bool repeated_ = false;

	public:
		COMPILE_TIME_ASSERT(std::is_void_v<typename event_data_type<event__type>::data_type>, "CORE -> COMPILE-TIME-ERROR -> 'unsupported/unmapped' event_type -> this need implemenation !!!");
		using data_type = typename event_data_type<event__type>::data_type;


		const core::event_type type = event__type;
		data_type data;

		event(data_type const& event_data, bool is_repeated = false) noexcept 
			: data(event_data) , repeated_(is_repeated)
		{

		}

		~event() = default;

		// event functions
		inline void set_handled()       noexcept { this->handled_ = true; }

		inline bool is_handled()        noexcept { return this->handled_; }
		inline bool is_repeated_event() noexcept { return this->repeated_; }

	}; 
	// class event end

	
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
	namespace keyboard_event{
		
		typedef core::event<core::event_type::keypress_up>   key_up;
		typedef core::event<core::event_type::keypress_down> key_down;
		
	}

} // namespace core

namespace core  {

	typedef u64 event_listener_id;

	// template callback function used for event listening 
	template<core::event_type event__type>     // void function( core::event<event_type> const& event );  
	using event_listener_function = std::function<void(core::event<event__type> const& event_object)>;

	/*
		dispatcher: event-manager who manage event queues/triggers/listeners/...
	*/
	namespace dispatcher {

		template<core::event_type event__type> 
		event_listener_id start_listening(core::event_listener_function<event__type> listener_function) noexcept;

		template<core::event_type event__type> void stop_listening(event_listener_id id) noexcept;

		/*
			note: "trigger_event" will block "main-thread" from exection !
		          for none-blocking use "queue_event" !
		*/
		template<core::event_type event__type> 
		void trigger_event(core::event<event__type> const& event_object) noexcept;

		template<core::event_type event__type> 
		void queue_event(std::unique_ptr<core::event<event__type>> event_object) noexcept;

	} // dispatcher end

} // namespace core

#endif