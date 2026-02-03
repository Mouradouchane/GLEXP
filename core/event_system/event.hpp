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
#include "core/data_structres/arrays/dynamic_array.hpp"

// few macros for external instantiate
#define EXTERN_TEMPLATE_FOR_EVENT(TYPE) \
		extern template listener_id start_listening<TYPE>(event_listener_function<TYPE>); \
		extern template void stop_listening<TYPE>(listener_id); \
		extern template void trigger_event<TYPE>(core::event<TYPE> const& _event_); \
		extern template void queue_event<TYPE>(std::unique_ptr<core::event<TYPE>> _event_);


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
	note: structs for events data
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
// note: "event_data_type" just used to do type mapping in "class event" !
//       basically the type of the data in event gonna be determineted at compile-time 
//       based on "event_type"
// ========================================================================================
template<event_type etype> struct event_data_type {
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
	 
	
namespace core {

	template<event_type etype> class event {

	private:
		bool handled_  = false;
		bool repeated_ = false;

	public:
		COMPILE_TIME_ASSERT(std::is_void_v<typename event_data_type<etype>::data_type>, "COMPILE-TIME-ERROR: CORE -> 'unsupported' event_type !");
		using data_type = typename event_data_type<etype>::data_type;

		const event_type type = etype;
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
		static event_category category(event const& event_object) noexcept;
		static string category_to_string(event const& event_object) noexcept;
		static string type_to_string(event const& event_object) noexcept;

	}; // class event end


	/*
		=============== for unkown/custom events ================ 
	*/
	typedef core::event<event_type::unkown> unkown_event;

	/*
		=============== for mouse events ===============
	*/
	namespace mouse_event {
		typedef core::event<event_type::mouse_click_left>  left_click;
		typedef core::event<event_type::mouse_click_right> right_click;
		typedef core::event<event_type::mouse_scroll>      scroll;
	}

	/*
		=============== for keyboard events ===============
	*/
	namespace keyboard_event {
		typedef core::event<event_type::keypress_up>   key_up;
		typedef core::event<event_type::keypress_down> key_down;
	}

	/*
		=============== for window events =================
	*/
	namespace window_event {
		typedef core::event<event_type::window_open>   window_open;
		typedef core::event<event_type::window_resize> window_resize;
		typedef core::event<event_type::window_close>  window_close;
	}
	

	/*
		note: - callback function used for event listening .
		      - void function( core::event<event_type> const& _event_ );  
	*/ 
	template<event_type etype>     
	using listener_function = std::function<void(core::event<etype> const& _event_)>;

	// few typedef as shortcuts for "listener_function" =============================
	typedef listener_function<event_type::unkown>  unkown_listener;

	typedef listener_function<event_type::mouse_click_left>  left_click_listener;
	typedef listener_function<event_type::mouse_click_right> right_click_listener;
	typedef listener_function<event_type::mouse_scroll>      mouse_scroll_listener;

	typedef listener_function<event_type::keypress_up>       keypress_up_listener;
	typedef listener_function<event_type::keypress_down>     keypress_down_listener;

	typedef listener_function<event_type::window_open>       window_open_listener;
	typedef listener_function<event_type::window_close>      window_close_listener;
	typedef listener_function<event_type::window_resize>     window_resize_listener;
	// ===============================================================================


	// event's dispatcher
	namespace event_system { 

		DLL_API void init(u32 queues_size, u32 queues_resize_value) noexcept;

		/*
			start/stop listening to events
		*/
		template<event_type etype> 
		DLL_API listener_id start_listening(listener_function<etype> listener_function) noexcept;

		template<event_type etype> 
		DLL_API void stop_listening(listener_id id) noexcept;

		/*
			note: - "trigger_event" will block "main-thread" from exection !
		          - for none-blocking use "queue_event" !
		*/
		template<event_type etype> 
		DLL_API void trigger_event(core::event<etype> const& _event_) noexcept;

		template<event_type etype> 
		DLL_API void queue_event(std::unique_ptr<core::event<etype>> _event_) noexcept;


		// ==================================================================s
		EXTERN_TEMPLATE_FOR_EVENT(event_type::unkown);
		// window events
		EXTERN_TEMPLATE_FOR_EVENT(event_type::window_open);
		EXTERN_TEMPLATE_FOR_EVENT(event_type::window_resize);
		EXTERN_TEMPLATE_FOR_EVENT(event_type::window_close);
		// mouse events
		EXTERN_TEMPLATE_FOR_EVENT(event_type::mouse_click_left);
		EXTERN_TEMPLATE_FOR_EVENT(event_type::mouse_click_right);
		EXTERN_TEMPLATE_FOR_EVENT(event_type::mouse_scroll);
		// keyboard events
		EXTERN_TEMPLATE_FOR_EVENT(event_type::keypress_up);
		EXTERN_TEMPLATE_FOR_EVENT(event_type::keypress_down);

		// todo: instantiate the rest !!!
		
		// ==================================================================

	} // event_system end

} // namespace core end 


#include "event_impl.hpp"

#endif