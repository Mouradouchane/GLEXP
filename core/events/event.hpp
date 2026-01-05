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
#include "core/types.hpp"
#include "core/strings/string.hpp"

#define EVENT_DEFAULT_PARAMETERS_WITH_VALUES bool is_repeated = false

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

	};

	/*
		parent event object !
		note: if you want to make your own custom events , inherite from this class
	*/
	DLL_API_CLASS event{
		protected:
			bool handled_;
			bool repeated_;

		public:
			const core::event_type type_;
			const core::event_category category_;

			// constructor
			event(
				core::event_category category__ = core::event_category::none,
				core::event_type type__ = core::event_type::none,
				EVENT_DEFAULT_PARAMETERS_WITH_VALUES
			);

			// destructor
			virtual ~event() = default;

			// event public functions
			inline void set_handled()        noexcept;
			inline bool is_handled()         noexcept;
			inline bool is_repeated_event()  noexcept;

	}; // event class end

	/*
		=============== mouse events ===============
	*/
	namespace mouse_event {

		DLL_API_CLASS mouse_click_left : public core::event{
			public:
				i32 x_;
				i32 y_;

				 mouse_click_left(i32 x, i32 y, EVENT_DEFAULT_PARAMETERS_WITH_VALUES);
				~mouse_click_left() = default;
		};

		DLL_API_CLASS mouse_click_right : public core::event{
			public:
				i32 x_;
				i32 y_;

				 mouse_click_right(i32 x, i32 y, EVENT_DEFAULT_PARAMETERS_WITH_VALUES);
				~mouse_click_right() = default;
		};

		DLL_API_CLASS mouse_scroll : public core::event{
			public:
				i32 x_;
				i32 y_;
				i32 scroll_factor_;

				 mouse_scroll(i32 x, i32 y, i32 scroll_factor, EVENT_DEFAULT_PARAMETERS_WITH_VALUES);
				~mouse_scroll() = default;
		};

	} // ================== namespace mouse_event end ===================


	/*
		=============== keyboard events ===============
	*/
	namespace keyboard_event {

		DLL_API_CLASS keypress_up : public core::event{
			public:
				char key_;
				bool special_key_;

				 keypress_up(char keycode_, bool is_special_key, EVENT_DEFAULT_PARAMETERS_WITH_VALUES);
				~keypress_up() = default;
		};

		DLL_API_CLASS keypress_down : public core::event{
			public:
				char key_;
				bool special_key_;

				 keypress_down(char keycode_, bool is_special_key, EVENT_DEFAULT_PARAMETERS_WITH_VALUES);
				~keypress_down() = default;
		};

	} // ================ namespace keyboard_event end ==================

} // namespace core

namespace core  {

	typedef u64 listener_id;

	// void function( core::event const& event );   use it to listen to events
	typedef std::function<void(const core::event&)> event_listner_function;

	/*
		dispatcher: event-manager who manage event queues/triggers/listeners/...
	*/
	namespace dispatcher {

		// event listening
		DLL_API listener_id listen(core::event_type event_type, core::event_listner_function listner_function);
		DLL_API void stop_listen(listener_id listener_id_);

		// note: this will block "main-thread" from exection
		DLL_API void trigger_event(core::event const& event_object);

		// note: this will not block "main-thread" from exection
		DLL_API void queue_event(std::unique_ptr<core::event> event_object);

	} // dispatcher end

} // namespace core

#endif