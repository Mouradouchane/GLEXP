#pragma once 

#ifndef EVENTS_CPP
#define EVENTS_CPP

#include "core/data_structres/arrays/dynamic_array.hpp"
#include "event.hpp"

#define DEFAULT_QUEUE_SIZE   16
#define DEFAULT_QUEUE_RESIZE  8

// macro for explicit template instantiate
#define INSTANTIATE_TEMPLATE_FOR_EVENT(TYPE) \
		template DLL_API listener_id start_listening<TYPE>(listener_function<TYPE>) noexcept; \
		template DLL_API void stop_listening<TYPE>(listener_id) noexcept; \
		template DLL_API void trigger_event<TYPE>(core::event<TYPE> const& _event_) noexcept; \
		template DLL_API void queue_event<TYPE>(std::unique_ptr<core::event<TYPE>> _event_) noexcept;

namespace core {
namespace event_system {
	
	static bool is_init = false;

	// queues of listeners
	namespace listeners {	
		core::dynamic_array<listener::unkown_event> unkown(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);

		core::dynamic_array<listener::mouse_left_click>   mouse_left_click(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<listener::mouse_right_click>  mouse_right_click(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<listener::mouse_scroll>       mouse_scroll(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);

		core::dynamic_array<listener::keypress_up>   keypress_up(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<listener::keypress_down> keypress_down(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);

		core::dynamic_array<listener::window_open>   window_open(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<listener::window_close>  window_close(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<listener::window_resize> window_resize(DEFAULT_QUEUE_SIZE, DEFAULT_QUEUE_RESIZE);
	} 

	// todo: add memory allocator for event system
	DLL_API void init(u32 queues_size, u32 queues_resize_value = DEFAULT_QUEUE_RESIZE) noexcept {

		if (!event_system::is_init) {
			queues_size = queues_size ? queues_size : DEFAULT_QUEUE_SIZE;
			queues_resize_value = queues_resize_value ? queues_resize_value : DEFAULT_QUEUE_RESIZE;

			// allocate arrays for event-listeners
			listeners::unkown = core::dynamic_array<listener::unkown_event>(queues_size, queues_resize_value);

			listeners::mouse_left_click  = core::dynamic_array<listener::mouse_left_click>(queues_size, queues_resize_value);
			listeners::mouse_right_click = core::dynamic_array<listener::mouse_right_click>(queues_size, queues_resize_value);
			listeners::mouse_scroll      = core::dynamic_array<listener::mouse_scroll>(queues_size, queues_resize_value);

			listeners::keypress_up   = core::dynamic_array<listener::keypress_up>(queues_size, queues_resize_value);
			listeners::keypress_down = core::dynamic_array<listener::keypress_down>(queues_size, queues_resize_value);

			listeners::window_open   = core::dynamic_array<listener::window_open>(queues_size, queues_resize_value);
			listeners::window_close  = core::dynamic_array<listener::window_close>(queues_size, queues_resize_value);
			listeners::window_resize = core::dynamic_array<listener::window_resize>(queues_size, queues_resize_value);

			event_system::is_init = true;
		}

	}

	template<event_type etype> 
	DLL_API listener_id start_listening(listener_function<etype> listener_function) noexcept {
		return NULL;
	}

	template<event_type etype> 
	DLL_API void stop_listening(listener_id id) noexcept {

	}

	template<event_type etype> 
	DLL_API void trigger_event(core::event<etype> const& _event_) noexcept {

	}

	template<event_type etype> 
	DLL_API void queue_event(std::unique_ptr<core::event<etype>> _event_) noexcept {

	}
	
	// ================== instantiate functions for events ===================
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::unkown);
	
	// window events
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::window_open);
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::window_resize);
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::window_close);
	
	// mouse events
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::mouse_click_left);
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::mouse_click_right);
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::mouse_scroll);
	
	// keyboard events
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::keypress_up);
	INSTANTIATE_TEMPLATE_FOR_EVENT(event_type::keypress_down);
	
	// todo: instantiate the rest !!!
	
	// ==================================================================
	
} // namespace event_system end
} // namespace core end 


#endif