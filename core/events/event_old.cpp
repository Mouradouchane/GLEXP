#pragma once 

#ifndef CORE_EVENTS_CPP
#define CORE_EVENTS_CPP

#include <unordered_map>

#include "core/status/status.hpp"
#include "core/memory/memory.hpp"
#include "core/data_structres/arrays/dynamic_array.hpp"
#include "event_old.hpp"

#ifdef DEBUG
	static bool logs_enabled = true;
#endif

#define DEFAULT_LISTENER_ARRAY_SIZE   16
#define DEFAULT_LISTENER_ARRAY_RESIZE  8

// macro for explicit template instantiate
#define INSTANTIATE_TEMPLATE_FOR_EVENT(TYPE) \
		template DLL_API listener_id start_listen<TYPE>(std::function<void(core::event<TYPE> const& _event_)> const&) noexcept; \
		template DLL_API bool stop_listen<TYPE>(listener_id id) noexcept; \
		template DLL_API void trigger_event<TYPE>(core::event<TYPE> const& _event_) noexcept; \
		template DLL_API void queue_event<TYPE>(core::event<TYPE> _event_) noexcept;


#define   LISTENER_FUNCTION(EVENT_TYPE) std::function<void(core::event<core::event_type::EVENT_TYPE> const& _event_)>
#define LISTENER_FUNCTION_E(EVENT_TYPE) std::function<void(core::event<EVENT_TYPE> const& _event_)>

#define   LISTENER_ARRAY(EVENT_TYPE) core::dynamic_array<   LISTENER_FUNCTION(EVENT_TYPE) >
#define LISTENER_ARRAY_E(EVENT_TYPE) core::dynamic_array< LISTENER_FUNCTION_E(EVENT_TYPE) >

// macro as shortcut for initializing "listeners-array"
#define INIT_LISTENER_ARRAY(EVENT_TYPE, LISTENER_ARRAY_PTR) \
		 LISTENER_ARRAY_PTR = (LISTENER_ARRAY(EVENT_TYPE)*) core::memory::allocate( sizeof(LISTENER_ARRAY(EVENT_TYPE)) ); \
		new(LISTENER_ARRAY_PTR) LISTENER_ARRAY(EVENT_TYPE)(size , resize); \
		 __listeners__::__map__.insert( { core::event_type::EVENT_TYPE , (void*)LISTENER_ARRAY_PTR } );


namespace __listeners__ {
	// unkown events
	LISTENER_ARRAY(unkown)* unkown;
	// window events
	LISTENER_ARRAY(window_open)*   window_open;
	LISTENER_ARRAY(window_resize)* window_resize;
	LISTENER_ARRAY(window_close)*  window_close;
	// mouse events
	LISTENER_ARRAY(mouse_left_click)*  mouse_left_click;
	LISTENER_ARRAY(mouse_right_click)* mouse_right_click;
	LISTENER_ARRAY(mouse_scroll)*      mouse_scroll;
	// keyboard events
	LISTENER_ARRAY(keypress_up)*   keypress_up;  
	LISTENER_ARRAY(keypress_down)* keypress_down;

	// todo: create the rest of listeners


	// listeners map using 
	std::unordered_map<core::event_type, void*> __map__;
};

namespace core {

namespace event_system {
	
	static bool is_init = false;
	static u32  listener_array_size   = 32u;
	static u32  listener_array_resize = 16u;

	// todo: add memory allocator for event system
	DLL_API void init() noexcept {
		CORE_TRACE_CURRENT_FUNCTION();

		if (!event_system::is_init) {
			u32 size = listener_array_size, resize = listener_array_resize;
			
			// init "__map__" map
			__listeners__::__map__ = std::unordered_map<core::event_type, void*>();

			// init listener's arrays + insert them to "__map__"

			// unkown event
			INIT_LISTENER_ARRAY(unkown,            __listeners__::unkown);
			// window events
			INIT_LISTENER_ARRAY(window_open,       __listeners__::window_open);
			INIT_LISTENER_ARRAY(window_resize,     __listeners__::window_resize);
			INIT_LISTENER_ARRAY(window_close,      __listeners__::window_close);
			// mouse events
			INIT_LISTENER_ARRAY(mouse_left_click,  __listeners__::mouse_left_click);
			INIT_LISTENER_ARRAY(mouse_right_click, __listeners__::mouse_right_click);
			INIT_LISTENER_ARRAY(mouse_scroll,      __listeners__::mouse_scroll);
			// keyboard events
			INIT_LISTENER_ARRAY(keypress_up,       __listeners__::keypress_up);
			INIT_LISTENER_ARRAY(keypress_down,     __listeners__::keypress_down);

			// todo: init the rest of __map__
				
			/*
			// files events
			INIT_LISTENER_ARRAY(file_open,          __listeners__:: ),
			INIT_LISTENER_ARRAY(file_close,         __listeners__:: ),
			INIT_LISTENER_ARRAY(file_write,         __listeners__:: ),
			INIT_LISTENER_ARRAY(file_read,          __listeners__:: ),
			INIT_LISTENER_ARRAY(file_exist,         __listeners__:: ),
			INIT_LISTENER_ARRAY(file_not_exist,     __listeners__:: ),
			// memory events
			INIT_LISTENER_ARRAY(memory_allocated,   __listeners__:: ),
			INIT_LISTENER_ARRAY(memory_deallocated, __listeners__:: ),
			*/
		
			event_system::is_init = true;
			CORE_DEBUG("listeners arrays starts with size={} , resize={}", size, resize);
		}
		else {		
			CORE_INFO("event_system is ready for usage !");
		}

		listener_id handle1 = core::event_system::start_listen<core::event_type::keypress_down>(
			[=](core::keyboard_event::keypress_down _event_) -> void {
				CORE_INFO("listener for {} is called !" , core::to_string(_event_.type));
			}
		);

		listener_id u_handle = core::event_system::start_listen<core::event_type::unkown>(
			[=](core::unkown_event _event_) -> void {
				CORE_WARN(
					"unkown listener is called -> event_data[data={} size={}]" ,
					(void*)_event_.data.data , _event_.data.size
				);
			}
		);

		core::event_system::stop_listen<core::event_type::unkown>(handle1);
		core::event_system::stop_listen<core::event_type::unkown>(listener_id{4,2});
		core::event_system::stop_listen<core::event_type::unkown>(u_handle);
		
	} 
	// init function end


	/*
			event_system functions implmentation
	*/


	template<core::event_type etype> 
	DLL_API listener_id start_listen (
		std::function<void(core::event<etype> const& _event_)> const& listener_function
	) noexcept {
		CORE_TRACE_CURRENT_FUNCTION();

		listener_id handle = { (u16)etype , 0 };

		// get listeners array from the map
		LISTENER_ARRAY_E(etype)* listeners = (LISTENER_ARRAY_E(etype)*)__listeners__::__map__.at(etype);

		DEBUG_BREAK;

		// try to find empty place in array 
		LISTENER_FUNCTION_E(etype)* listener;
		for (u32 i = 0; i < listeners->size(); i++ ) {
			listener = &listeners->get(i);

			if (*listener) continue;
			else {
				// insert & save index
				*listener = listener_function;
				handle.index2 = i;

				CORE_INFO(
					" new listener for {} event | id=({},{})" ,
					core::to_string(etype) , handle.index1 , handle.index2
				);

				return handle;
			}
		}

		// else push new listener and save index
		handle.index2 = listeners->push( listener_function );

		CORE_INFO(
			" new listener for {} event | id=({},{})" ,
			core::to_string(etype) , handle.index1 , handle.index2
		);

		return handle;
	}

	template<core::event_type etype>
	DLL_API bool stop_listen(listener_id id) noexcept {
		CORE_TRACE_CURRENT_FUNCTION();
		DEBUG_BREAK;

		return 0;
		/*
		DEBUG_BREAK;

		// get listeners array from the map
		auto listeners = (core::dynamic_array<std::function<void(core::event<etype> const&)>>*) __map__.at((core::event_type)((u16)id.index1));

		// check if index2 is out of range
		if (id.index2 >= listeners->size()) {
			CORE_ERROR(, core::status::get_error(core::error::index_out_range));
			CORE_WARN(, " handle.index2 is out of range !");
			CORE_WARN(core::status::get_warning(core::warning::segfault_crash));

			CORE_CRASH();
			return false;
		}

		// remove listener "stop listening"
		if (listeners[handle.index2]) {
			CORE_DEBUG( , " function {} stoped listening for event {}",
				(void*)&listeners[handle.index2], core::to_string(etype)
			);

			listeners[handle.index2] = nullptr;
			return true;
		}
		else { // already deleted
			CORE_WARN( , " listener function already deleted !");
			return false;
		}
		*/
	}

	template<core::event_type etype> 
	DLL_API void trigger_event(core::event<etype> const& _event_) noexcept {
		// todo: implement this

		CORE_CRASH(core::status::get_error(core::error::not_implemented_yet));
	}

	template<core::event_type etype> 
	DLL_API void queue_event(core::event<etype> _event_) noexcept {
		// todo: implement this

		CORE_CRASH(core::status::get_error(core::error::not_implemented_yet));
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