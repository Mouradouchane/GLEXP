#pragma once 

#ifndef CORE_EVENTS_CPP
#define CORE_EVENTS_CPP

#include <unordered_map>
#include "core/data_structres/arrays/dynamic_array.hpp"
#include "event.hpp"

#define DEFAULT_LISTENER_ARRAY_SIZE   16
#define DEFAULT_LISTENER_ARRAY_RESIZE  8

// macro for explicit template instantiate
#define INSTANTIATE_TEMPLATE_FOR_EVENT(TYPE) \
		template DLL_API listener_id start_listening<TYPE>(const core::listener_function<TYPE>&) noexcept; \
		template DLL_API void stop_listening<TYPE>(listener_id) noexcept; \
		template DLL_API void trigger_event<TYPE>(core::event<TYPE> const& _event_) noexcept; \
		template DLL_API void queue_event<TYPE>(std::unique_ptr<core::event<TYPE>> _event_) noexcept;

// macro as shortcut for creating listener array as std::pair<core::event_type, void*>
#define INIT_LISTENER_ARRAY(EVENT_TYPE, LISTENER_TYPE , ARR_SIZE , ARR_RESIZE) \
		{ EVENT_TYPE , (void*)&core::dynamic_array<LISTENER_TYPE>(ARR_SIZE, ARR_RESIZE) },



// map of array of listeners
// arrays casted as void*
static std::unordered_map<core::event_type, void*> listeners;

namespace core {
namespace event_system {
	
	static bool is_init = false;
	static u32  listener_array_size   = 32u;
	static u32  listener_array_resize = 16u;

	// todo: add memory allocator for event system
	DLL_API void init() noexcept {

		if (!event_system::is_init) {
			u32 size = listener_array_size, resize = listener_array_resize;
			
			// init "listeners" map
			listeners = std::unordered_map<core::event_type, void*>();

			// todo: create memory-allocator for listeners when allocator is ready to use
			
			// create/init listeners arrays
			listeners.insert({
				// unkown event
				INIT_LISTENER_ARRAY(core::event_type::unkown, core::listener::unkown_event, size, resize)
				// window events
				INIT_LISTENER_ARRAY(core::event_type::window_open, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::window_resize, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::window_close, core::listener::unkown_event, size, resize)
				// mouse events
				INIT_LISTENER_ARRAY(core::event_type::mouse_click_left, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::mouse_click_right, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::mouse_scroll, core::listener::unkown_event, size, resize)
				// keyboard events
				INIT_LISTENER_ARRAY(core::event_type::keypress_up, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::keypress_down, core::listener::unkown_event, size, resize)
				// files events
				INIT_LISTENER_ARRAY(core::event_type::file_open, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_close, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_write, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_read, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_exist, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_not_exist, core::listener::unkown_event, size, resize)
				// memory events
				INIT_LISTENER_ARRAY(core::event_type::memory_allocated, core::listener::unkown_event, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::memory_deallocated, core::listener::unkown_event, size, resize)
			});

			event_system::is_init = true;
		}

	} // init function end

	template<core::event_type etype> 
	DLL_API listener_id start_listening(core::listener_function<etype> const& listener_function) noexcept {

		// get listener pointer
		auto plistener = listeners.at(etype);

		// check if listener is full

		// search for empty place

		// insert new listener

		// return index

		return NULL;
	}

	template<core::event_type etype> 
	DLL_API void stop_listening(listener_id id) noexcept {

	}

	template<core::event_type etype> 
	DLL_API void trigger_event(core::event<etype> const& _event_) noexcept {

	}

	template<core::event_type etype> 
	DLL_API void queue_event(std::unique_ptr<core::event<etype>> _event_) noexcept {

	}
	
	// ================== instantiate functions for events ===================
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::unkown);
	
	// window events
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::window_open);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::window_resize);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::window_close);
	
	// mouse events
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::mouse_click_left);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::mouse_click_right);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::mouse_scroll);
	
	// keyboard events
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::keypress_up);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::keypress_down);
	
	// todo: instantiate the rest !!!
	
	// ==================================================================
	
} // namespace event_system end
} // namespace core end 


#endif