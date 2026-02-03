#pragma once 

#ifndef EVENTS_CPP
#define EVENTS_CPP

#include "event.hpp"

#define DEFAULT_QUEUE_SIZE   16
#define DEFAULT_QUEUE_RESIZE  8

// macro for explicit template instantiate
#define INSTANTIATE_TEMPLATE_FOR_EVENT(TYPE) \
		template DLL_API listener_id start_listening<TYPE>(event_listener_function<TYPE>); \
		template DLL_API void stop_listening<TYPE>(listener_id); \
		template DLL_API void trigger_event<TYPE>(core::event<TYPE> const& _event_); \
		template DLL_API void queue_event<TYPE>(std::unique_ptr<core::event<TYPE>> _event_);

namespace core {
namespace event_system {
	
	static bool is_init = false;

	// queues of listeners
	namespace listeners {	
		core::dynamic_array<unkown_listener>        unkown(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);

		core::dynamic_array<left_click_listener>    left_click(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<right_click_listener>   right_click(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<mouse_scroll_listener>  mouse_scroll(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);

		core::dynamic_array<keypress_up_listener>   keypress_up(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<keypress_down_listener> keypress_down(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);

		core::dynamic_array<window_open_listener>   window_open(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<window_close_listener>  window_close(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);
		core::dynamic_array<window_resize_listener> window_resize(DEFAULT_QUEUE_SIZE , DEFAULT_QUEUE_RESIZE);
	} 

	// todo: add memory allocator for event system
	DLL_API void init(u32 queues_size, u32 queues_resize_value = DEFAULT_QUEUE_RESIZE) noexcept {

		if (!event_system::is_init) {
			queues_size = queues_size ? queues_size : DEFAULT_QUEUE_SIZE;
			queues_resize_value = queues_resize_value ? queues_resize_value : DEFAULT_QUEUE_RESIZE;

			// allocate memory for listeners
			listeners::unkown = core::dynamic_array<unkown_listener>(queues_size, queues_resize_value);

			listeners::left_click = core::dynamic_array<left_click_listener>(queues_size, queues_resize_value);
			listeners::right_click = core::dynamic_array<right_click_listener>(queues_size, queues_resize_value);
			listeners::mouse_scroll = core::dynamic_array<mouse_scroll_listener>(queues_size, queues_resize_value);

			listeners::keypress_up = core::dynamic_array<keypress_up_listener>(queues_size, queues_resize_value);
			listeners::keypress_down = core::dynamic_array<keypress_down_listener>(queues_size, queues_resize_value);

			listeners::window_open = core::dynamic_array<window_open_listener>(queues_size, queues_resize_value);
			listeners::window_close = core::dynamic_array<window_close_listener>(queues_size, queues_resize_value);
			listeners::window_resize = core::dynamic_array<window_resize_listener>(queues_size, queues_resize_value);

			event_system::is_init = true;
		}

	}

	template<event_type etype> 
	DLL_API listener_id start_listening(event_listener_function<etype> listener_function) noexcept {

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