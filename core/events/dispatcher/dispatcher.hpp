#pragma once 

#ifndef CORE_DISPATCHER_HPP
#define CORE_DISPATCHER_HPP

#include <functional>

#include "core/memory/memory.hpp"
#include "core/data_structres/arrays/dynamic_array.hpp"

#ifdef DEBUG
	static bool logs_enabled = true;
#endif

namespace core {

	template<typename type> using callback = std::function<void(type const& data)>;


	template<typename type> class dispatcher {
		private:
			core::dynamic_array< core::callback<type> > _listeners_;

		public:
			// constructor/destructor
			 dispatcher(u32 count, u32 resize_value , core::memory_allocator* allocator = nullptr);
			~dispatcher();

			// dispatcher public functions
			u32  subscribe(core::callback<type> const& callback_function);
			bool unsubscribe(u32 index);

			void trigger(u32 index, type const& data); 
			void trigger_all(type const& data);
		
			void clear();

			// note/todo : maybe later --> queue(data); for multi-threading

	}; // dispatcher class end

} // core namespace end


/*
	 =========== dispatcher implementation ============
*/


namespace core {

// constructor 
template<typename type> dispatcher<type>::dispatcher(
	u32 count , u32 resize_value , core::memory_allocator* allocator = nullptr
) {
	this->_listeners_ = core::dynamic_array<type>(count, resize_value ,allocator);
	CORE_DEBUG("dispatcher constructed <{}>",typeid(type).name());
}

// destructor
template<typename type> dispatcher<type>::~dispatcher() {

	this->_listeners_.~dynamic_array();
	CORE_DEBUG("dispatcher destructed <{}>", typeid(type).name());
}

/*
	====== dispatcher public functions ======
*/ 

template<typename type>
u32 dispatcher<type>::subscribe(callback<type> const& callback_function) noexcept {

	return this->_listeners_.push(callback_function);
}

template<typename type> bool dispatcher<type>::unsubscribe(u32 index) noexcept {

	if (index >= _listeners_.size()) {
		CORE_ERROR_D(core::status::get_error(core::error::index_out_range),index , _listeners_.size());
		return false;
	}

	std::memset((void*)&this->_listeners_[index], 0, sizeof(core::callback<type>));
	return true;
}

// call all listeners 
template<typename type> void dispatcher<type>::trigger_all(type const& data) noexcept {

	for (u32 i = 0; i < this->_listeners_.size(); i++ ) {
		if (this->_listeners_[i]) {
			this->_listeners_[i](data);
		}
	}

	CORE_DEBUG_D("all listeners for <{}> in {} triggered !" , typeid(type).name() , (void*)this);
}

// call specific listener
template<typename type> void dispatcher<type>::trigger(u32 index, type const& data) noexcept {
	
	if (index >= _listeners_.size()) {
		CORE_ERROR_D(core::status::get_error(core::error::index_out_range), index);
		return;
	}

	if (this->_listeners_[index]) {
		this->_listeners_[index](data);
		return;
	}
	else {
		CORE_FATAL_D(core::status::get_error(core::error::listener_not_found) , index);
		CORE_CRASH();
	}

}

template<typename type> void dispatcher<type>::clear() noexcept {
	this->_listeners_.clear();
	CORE_INFO_D("all listeners in {} for {} is deleted !" , (void*)this , typeid(type).name());
}

} // namespace core end
#endif 