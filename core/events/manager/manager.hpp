#pragma once 

#ifndef CORE_EVENT_SYSTEM_HPP
#define CORE_EVENT_SYSTEM_HPP

#include <unordered_map>
#include "core/strings/string.hpp"
#include "core/events/dispatcher/dispatcher.hpp"

#ifdef DEBUG
	static bool logs_enabled = true;
#endif

namespace core {

	enum class events_category : u16 {
		unspecified_events = 0,
		mouse_events,
		keyboard_events,
		window_events,
		ui_events,
		audio_events,
		graphics_events,
		physics_events,
		collision_events,
		ai_events,

	#ifdef DEBUG
		memory_events,
		dev_events,
	#endif
	};

	class event_manager {
	private:
		static const u32 _default_size_   = 64u;
		static const u32 _default_resize_ = 32u;

	private:
		STRING _name_;
		u32    _size_; // for 
		u32    _resize_;
		core::events_category _category_; 
		core::memory_allocator* _allocator_ = nullptr;
		std::unordered_map<u32, void*> _listeners_map_; // map of core::dispatchers

	public:
		// constructor/destructor
		event_manager(
			core::events_category category, STRING name,
			u32 size = _default_size_, u32 resize_value = _default_resize_,
			core::memory_allocator* allocator = nullptr
		);
		~event_manager();

		// manager public functions
		template<typename type> listener_id start_listen(core::callback<type> const& callback_function);
		bool stop_listen(listener_id id);

		template<typename type> void trigger_all(type const& data);
		template<typename type> void trigger(listener_id id, type const& data);

		template<typename type> void queue_all(type data);
		template<typename type> void queue(listener_id id, type const& data);

	}; // class event_system end

} // namespace core end


/*
	========= event_manager implementation ==========
*/

namespace core {

event_manager::event_manager(
	core::events_category category, STRING name, u32 size = _default_size_, u32 resize_value = _default_resize_ ,
	core::memory_allocator* allocator = nullptr
) 
	: _category_(category) , _name_(name) , _size_(size) , _resize_(resize_value) , _allocator_(allocator)
{
	this->_listeners_map_ = std::unordered_map<u32, void*>(32u);
	CORE_DEBUG("event_manager constructed");
}

event_manager::~event_manager() {

	this->_listeners_map_.clear();
	CORE_DEBUG("event_manager destructed");
}

/*
	event_manager functions
*/

template<typename type> listener_id event_manager::start_listen<type>(core::callback<type> const& callback_function) noexcept {



}


} // namespace core end

#endif