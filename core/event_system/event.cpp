#pragma once 

#ifndef CORE_EVENTS_CPP
#define CORE_EVENTS_CPP

#include <unordered_map>

#include "core/status/status.hpp"
#include "core/data_structres/arrays/dynamic_array.hpp"
#include "event.hpp"

#define DEFAULT_LISTENER_ARRAY_SIZE   16
#define DEFAULT_LISTENER_ARRAY_RESIZE  8

// macro for explicit template instantiate
#define INSTANTIATE_TEMPLATE_FOR_EVENT(TYPE) \
		template DLL_API event_handle start_listen<TYPE>(std::function<void(core::event<TYPE> const& _event_)> const&) noexcept; \
		template DLL_API bool stop_listen<TYPE>(event_handle) noexcept; \
		template DLL_API void trigger_event<TYPE>(core::event<TYPE> const& _event_) noexcept; \
		template DLL_API void queue_event<TYPE>(core::event<TYPE> _event_) noexcept;


// macro as shortcut for creating event_listener array as std::pair<core::event_type, void*>
#define INIT_LISTENER_ARRAY(EVENT_TYPE, LISTENER_TYPE , ARR_SIZE , ARR_RESIZE) \
		{ EVENT_TYPE , (void*)&core::dynamic_array<LISTENER_TYPE>(ARR_SIZE, ARR_RESIZE) },


// map of array of listeners_map
// arrays casted as void*
static std::unordered_map<core::event_type, void*> listeners_map;

namespace core {

namespace event_system {
	
	static bool is_init = false;
	static u32  listener_array_size   = 32u;
	static u32  listener_array_resize = 16u;

	// todo: add memory allocator for event system
	DLL_API void init() noexcept {

		if (!event_system::is_init) {
			u32 size = listener_array_size, resize = listener_array_resize;
			
			// init "listeners_map" map
			listeners_map = std::unordered_map<core::event_type, void*>();

			// todo: create memory-allocator for listeners_map when allocator is ready to use
			
			// create/init listeners_map arrays
			listeners_map.insert({
				// unkown event
				INIT_LISTENER_ARRAY(core::event_type::unkown, core::event_listener::unkown_event, size, resize)
				// window events
				INIT_LISTENER_ARRAY(core::event_type::window_open,   core::event_listener::window_open, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::window_resize, core::event_listener::window_resize, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::window_close,  core::event_listener::window_close, size, resize)
				// mouse events
				INIT_LISTENER_ARRAY(core::event_type::mouse_left_click,  core::event_listener::mouse_left_click , size, resize)
				INIT_LISTENER_ARRAY(core::event_type::mouse_right_click, core::event_listener::mouse_right_click, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::mouse_scroll,      core::event_listener::mouse_scroll, size, resize)
				// keyboard events
				INIT_LISTENER_ARRAY(core::event_type::keypress_up,   core::event_listener::keypress_up, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::keypress_down, core::event_listener::keypress_down, size, resize)

				/*
				// todo: the rest of listeners_map need implementation 
				
				// files events
				INIT_LISTENER_ARRAY(core::event_type::file_open, core::event_listener::, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_close, core::event_listener::, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_write, core::event_listener::, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_read, core::event_listener::, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_exist, core::event_listener::, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::file_not_exist, core::event_listener::, size, resize)
				// memory events
				INIT_LISTENER_ARRAY(core::event_type::memory_allocated, core::event_listener::, size, resize)
				INIT_LISTENER_ARRAY(core::event_type::memory_deallocated, core::event_listener::, size, resize)
				*/
			});

			event_system::is_init = true;

			CORE_DEBUG(FUNCTION_DEFINITION);
			CORE_DEBUG("listeners arrays starts with size={} , resize={}", size, resize);

		}
		else {
			CORE_INFO(FUNCTION_DEFINITION);
			CORE_INFO("event_system is ready for usage !");
		}

		DEBUG_BREAK;
		event_handle handle1 = core::event_system::start_listen<core::event_type::keypress_down>(
			[=](core::keyboard_event::keypress_down _event_) -> void {
				CORE_INFO("listener for {} is called !" , core::to_string(_event_.type));
			}
		);

		event_handle u_handle = core::event_system::start_listen<core::event_type::unkown>(
			[=](core::unkown_event _event_) -> void {
				CORE_WARN(
					"unkown listener is called -> event_data[data={} size={}]" ,
					(void*)_event_.data.data , _event_.data.size
				);
			}
		);

		core::event_system::stop_listen<core::event_type::keypress_down>(handle1);
		core::event_system::stop_listen<core::event_type::keypress_down>(event_handle{4,2});
		core::event_system::stop_listen<core::event_type::unkown>(u_handle);
		
	} 
	// init function end


	/*
			event_system functions implmentation
	*/

	template<core::event_type etype> DLL_API event_handle start_listen(
		std::function<void(core::event<etype> const& _event_)> const& listener_function
	) noexcept {
		
		DEBUG_BREAK;
		event_handle handle = { (u16)etype , 0 };

		// get listeners array from the map
		core::dynamic_array< std::function<void(core::event<etype> const&)> >& 
		listeners_array = (core::dynamic_array< std::function<void(core::event<etype> const&)> > &)listeners_map.at(etype);

		// try to find empty place in array 
		for (std::function<void(core::event<etype> const& _event_)>& listener : listeners_array) {

			if (listener.target<void(core::event<etype> const& _event_)>() != nullptr) handle.index2 += 1;
			else {
				// insert & save index
				listeners_array[handle.index2] = listener_function;
				return handle;
			}
		}

		// else push new listener and save index
		handle.index2 = listeners_array.push( listener_function );

		CORE_DEBUG(
			FUNCTION_DEFINITION, " 0x{} function start listening to {} event ." , 
			(void*)&listener_function , core::to_string(etype)
		);

		return handle;
	}

	template<core::event_type etype> 
	DLL_API bool stop_listen(event_handle handle) noexcept {

		DEBUG_BREAK;
		
		// note : index1 != etype -> means wrong handle is passed to this function !
		if(handle.index1 != ((u16)etype)) {
			CORE_ERROR(core::status::get_error(core::error::invalid_handle) );
			CORE_WARN("handle.index1 is for {}, but expected to be {} !" , core::to_string((core::event_type)((u16)handle.index1)), (u16)etype);
			CORE_WARN(core::status::get_warning(core::warning::runtime_crash));

			CORE_CRASH();
			return false;
		}

		// get listeners array from the map
		core::dynamic_array< std::function<void(core::event<etype> const&)> >& 
		listeners_array = (core::dynamic_array< std::function<void(core::event<etype> const&)> > &)listeners_map.at(etype);

		// check if index2 is out of range
		if (listeners_array.size() <= handle.index2) {
			CORE_ERROR(FUNCTION_DEFINITION, core::status::get_error(core::error::index_out_range));
			CORE_WARN(FUNCTION_DEFINITION, " handle.index2 is out of range !");
			CORE_WARN(core::status::get_warning(core::warning::segfault_crash));

			CORE_CRASH();
			return false;
		}

		// remove listener "stop listening"
		if (listeners_array[handle.index2]) {
			CORE_DEBUG(FUNCTION_DEFINITION , " function 0x{} stoped listening for event {}",
				(void*)&listeners_array[handle.index2], core::to_string(etype)
			);

			listeners_array[handle.index2] = nullptr;
			return true;
		}
		else { // already deleted
			CORE_WARN(FUNCTION_DEFINITION , " listener function already deleted !");
			return false;
		}

	}

	template<core::event_type etype> 
	DLL_API void trigger_event(core::event<etype> const& _event_) noexcept {

	}

	template<core::event_type etype> 
	DLL_API void queue_event(core::event<etype> _event_) noexcept {

	}
	
	// ================== instantiate functions for events ===================
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::unkown);
	
	// window events
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::window_open);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::window_resize);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::window_close);
	
	// mouse events
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::mouse_left_click);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::mouse_right_click);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::mouse_scroll);
	
	// keyboard events
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::keypress_up);
	INSTANTIATE_TEMPLATE_FOR_EVENT(core::event_type::keypress_down);
	
	// todo: instantiate the rest !!!
	
	// ==================================================================
	
} 
// namespace event_system end


#define CASE_EVENT_TYPE_TO_STR(EVENT_TYPE) \
			case core::event_type:: EVENT_TYPE : return #EVENT_TYPE; break;

	DLL_API STRING to_string(core::event_type _type_) noexcept {

	switch (_type_) {
		// mouse
		CASE_EVENT_TYPE_TO_STR( mouse_scroll      );
		CASE_EVENT_TYPE_TO_STR( mouse_left_click  );
		CASE_EVENT_TYPE_TO_STR( mouse_right_click );
		// keyboard
		CASE_EVENT_TYPE_TO_STR( keypress_up   );
		CASE_EVENT_TYPE_TO_STR( keypress_down );
		// window
		CASE_EVENT_TYPE_TO_STR( window_open   );
		CASE_EVENT_TYPE_TO_STR( window_resize );
		CASE_EVENT_TYPE_TO_STR( window_close  );
		// files
		CASE_EVENT_TYPE_TO_STR( file_open      );
		CASE_EVENT_TYPE_TO_STR( file_close     );
		CASE_EVENT_TYPE_TO_STR( file_write	   );
		CASE_EVENT_TYPE_TO_STR( file_read	   );
		CASE_EVENT_TYPE_TO_STR( file_exist	   );
		CASE_EVENT_TYPE_TO_STR( file_not_exist );
		// memory
		CASE_EVENT_TYPE_TO_STR( memory_allocated   );
		CASE_EVENT_TYPE_TO_STR( memory_deallocated );

		// todo: implement the rest
		// controller
		// gui
		// graphics
		// physics
		// animation
		// network

		default : return "unkown";
	}

}


#define CASE_EVENT_CATEGORY_TO_STRING(EVENT_CATEGORE) \
			case core::event_category:: EVENT_CATEGORE : return #EVENT_CATEGORE; break;

	DLL_API STRING to_string(core::event_category _category_) noexcept {

	switch (_category_) {

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

	default : return "unkown";
	}

}

} // namespace core end 


#endif