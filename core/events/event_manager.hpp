#pragma once 

#ifndef CORE_EVENT_MANAGER_HPP
#define CORE_EVENT_MANAGER_HPP

#include <unordered_map>

#include "core/macros.hpp"
#include "core/strings/string.hpp"
#include "core/events/event_dispatcher.hpp"

#ifdef DEBUG
	static auto _logger_ = CORE_GET_LOGGER( EVENT_SYSTEM_LOGGER );
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
	
	private: // private static configs
		static inline u32 _total_event_managers_ = 0u;
		static const  u32 _default_size_   = 32u;
		static const  u32 _default_resize_ = 32u;

		// this just to convert type T to integer-id
		template<typename T> static u32 get_type_id() {
			static const u32 id = _next_id_++;
			return id;
		}

		static inline u32 _next_id_ = 0; // note: need C++17 for inline

	private: // event manager private variables
		STRING _name_;
		u32    _size_;
		u32    _resize_;
		core::events_category _category_; 

		// todo: move from ptr to refcounted
		core::memory_allocator* _allocator_ = nullptr;

		// todo: "optimization" change from map to array for 0(1) insted of log(n) -> map.find( )
		std::unordered_map<u32, b_dispatcher*> _dispatchers_map_;

	public: // event manager public function

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

		template<typename type> void trigger_all(type const& data) noexcept;
		template<typename type> void trigger(listener_id id, type const& data) noexcept;

		template<typename type> void queue_all(type data) noexcept; 
		template<typename type> void queue(listener_id id, type const& data) noexcept;

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
	core::event_manager::_total_event_managers_ += 1;
	CORE_DEBUG("event_manager constructed");
}

event_manager::~event_manager() {
	
	// destruct all dispatchers
	if (this->_allocator_) {
		for (auto& [id, dispatcher] : this->_dispatchers_map_ ) {
			dispatcher->~b_dispatcher(); 
			this->_allocator_->deallocate((void*)dispatcher);
		}
	}
	else {
		for (auto& [id, dispatcher] : this->_dispatchers_map_ ) {
			dispatcher->~b_dispatcher(); 
			core::memory::deallocate((void*)dispatcher);
		}
	}

	if (core::event_manager::_total_event_managers_) {
		core::event_manager::_total_event_managers_ -= 1;
	}

	CORE_DEBUG("event_manager destructed");
}

/*
	event_manager functions
*/

template<typename type> 
listener_id event_manager::start_listen(core::callback<type> const& callback_function) noexcept {
	// get type id/index
	u32 type_index = core::event_manager::get_type_id<type>();

	listener_id id = { type_index , 0u };
	auto itr = this->_dispatchers_map_.find(id.index1);
	core::dispatcher<type>* dptr;

	DEBUG_BREAK;

	// if dispatcher not found
	if (itr == this->_dispatchers_map_.end()) {
		// allocate dispatcher
		void* mem;
		if (this->_allocator_) {
			mem = this->_allocator_->allocate(sizeof(core::dispatcher<type>));
		}
		else {
			mem = core::memory::allocate(sizeof(core::dispatcher<type>), core::memory::tag::event_system);
		}

		// construct dispatcher
		dptr = new (mem) core::dispatcher<type>(this->_size_, this->_resize_, this->_allocator_);
		this->_dispatchers_map_[id.index1] = dptr;
	}
	else dptr = static_cast<core::dispatcher<type>*>( itr->second );

	// insert "callback" in dispatcher
	id.index2 = dptr->subscribe(callback_function);
	CORE_TRACE("new listener start for <{}> id={}" , typeid(type).name(), id.index2);

	return id;
}

bool core::event_manager::stop_listen(listener_id id) {
	auto itr = this->_dispatchers_map_.find(id.index1);
	DEBUG_BREAK;

	if (itr == this->_dispatchers_map_.end()) {
		CORE_ERROR(core::status::get_error(core::error::index_out_range));
		CORE_CRASH();
		return false;
	}

#ifdef DEBUG
	bool unsub = itr->second->unsubscribe(id.index2);
	if (unsub) CORE_TRACE("listener {} stoped {}" , id.index2 , id.index1);
	return unsub;
#else 
	return itr->second->unsubscribe(id.index2);
#endif

}


template<typename type> 
void event_manager::trigger_all(type const& data) noexcept {
	u32  type_index = core::event_manager::get_type_id<type>();
	auto itr = this->_dispatchers_map_.find(type_index);
	
	DEBUG_BREAK;

	if (itr != this->_dispatchers_map_.end()) {
		core::dispatcher<type>* dis = static_cast<core::dispatcher<type>*>(itr->second);
		dis->trigger_all(data);
		return;
	}

	CORE_WARN("event triggered for <{}> but no dispatcher found to handled yet !" , typeid(type).name() );
}

template<typename type>
void event_manager::trigger(listener_id id, type const& data) noexcept {
	u32  type_index = core::event_manager::get_type_id<type>();
	
	DEBUG_BREAK;

	// note: if type_index not matching index1 , this usually a "usage-bug"
	if (type_index != id.index1) {
		CORE_WARN_D("listener id not matching with type id ? this is probablly a bug !");
		CORE_ERROR_D(core::status::get_error(core::error::invalid_id),id.index1);
		CORE_CRASH();
		return;
	}

	auto itr = this->_dispatchers_map_.find(type_index);
	if (itr != this->_dispatchers_map_.end()) {
		core::dispatcher<type>* dis = static_cast<core::dispatcher<type>*>(itr->second);
		dis->trigger(id.index2,data);
		return;
	}

	CORE_WARN("event triggered for <{}> but no dispatcher found to handled yet !" , typeid(type).name() );
}

// todo: implement this
template<typename type> 
void event_manager::queue_all(type data) noexcept {

	CORE_TRACE_CURRENT_FUNCTION();
	CORE_ASSERT(1, "todo:not implemented yet !");
}

// todo: implement this
template<typename type>
void event_manager::queue(listener_id id, type const& data) noexcept {

	CORE_TRACE_CURRENT_FUNCTION();
	CORE_ASSERT(1, "todo:not implemented yet !");
}


} // namespace core end

#endif