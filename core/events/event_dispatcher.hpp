#pragma once 

#ifndef CORE_DISPATCHER_HPP
#define CORE_DISPATCHER_HPP

#include <functional>

#include "core/macros.hpp"
#include "core/logger/logger.hpp"
#include "core/strings/string.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"
#include "core/containers/arrays/dynamic_array.hpp"

#ifdef DEBUG
	static auto _event_dispatcher_hpp_logger_ = CORE_GET_LOGGER( EVENT_SYSTEM_LOGGER );
#else 
	static auto _event_dispatcher_hpp_logger_ = nullptr;
#endif

#define _LOGGER_ _event_dispatcher_hpp_logger_

/*
	few macros for dispatcher errors/warnings 
*/
#define CORE_DISPATCHER_INDEX_OUT_OF_RANGE "out of range index {} passed to dispatcher {} !"
#define CORE_LISTENER_NOT_FOUND "listener {} is not found in dispatcher {} !"

namespace core {

	/*
		callback function used to recive event data
	*/
	template<typename type> using callback = std::function<void(type const& event_data)>;

	class b_dispatcher {
		public:
			virtual ~b_dispatcher() = default;
			virtual bool unsubscribe(u32 index) = NEED_IMPL;
	};

	template<typename type> 
	class dispatcher : public b_dispatcher {
		private:
			core::dynamic_array< core::callback<type> > _listeners_ = core::dynamic_array < core::callback<type>>(8,8,nullptr);

			// todo: add support for name
			DEBUG_ONLY string dispatcher_name = "unkown";

		public:
			// constructor/destructor
			 dispatcher(u32 count, u32 resize_value , core::dynamic_allocator* allocator = nullptr);
			~dispatcher() override;

			// dispatcher public functions
			u32    subscribe(core::callback<type> const& callback_function) NOEXP;
			bool unsubscribe(u32 index) NOEXP override;

			void trigger(u32 index, type const& data) NOEXP;
			void trigger_all(type const& data) NOEXP;
		
			void clear() NOEXP;

			// note/todo : maybe later --> queue(data); for multi-threading

	}; // dispatcher class end

} // core namespace end


/*
	 =========== dispatcher implementation ============
*/


namespace core {

// constructor 
template<typename type> 
dispatcher<type>::dispatcher(u32 count, u32 resize_value, core::dynamic_allocator* allocator) {

	this->_listeners_ = core::dynamic_array<core::callback<type>>(count, resize_value, allocator);
	CORE_DEBUG_D("dispatcher constructed <{}>",typeid(type).name());
}

// destructor
template<typename type> 
dispatcher<type>::~dispatcher() {
	CORE_DEBUG_D("dispatcher destructed <{}>", typeid(type).name());
}

/*
	====== dispatcher public functions ======
*/ 

template<typename type>
u32 dispatcher<type>::subscribe(callback<type> const& callback_function) NOEXP {

	return this->_listeners_.push(callback_function);
}


template<typename type> 
bool dispatcher<type>::unsubscribe(u32 index) NOEXP {

	if (index >= this->_listeners_.size()) {
	#ifdef DEBUG
		CORE_ERROR_F(CORE_DISPATCHER_INDEX_OUT_OF_RANGE , index, this->dispatcher_name);
	#endif
		return false;
	}

	this->_listeners_[index] = nullptr;
	return true;
}

// call all listeners 
template<typename type> 
void dispatcher<type>::trigger_all(type const& data) NOEXP {

	for (u32 i = 0; i < this->_listeners_.size(); i++ ) {
		if (this->_listeners_[i]) {
			this->_listeners_[i](data);
		}
	}

	CORE_DEBUG_D_D("all listeners for <{}> in {} triggered !" , typeid(type).name() , (void*)this);
}

// call specific listener
template<typename type> 
void dispatcher<type>::trigger(u32 index, type const& data) NOEXP {
	
	if (index >= _listeners_.size()) {
		CORE_ERROR_D(CORE_DISPATCHER_INDEX_OUT_OF_RANGE, index , this->dispatcher_name);
		return;
	}

	if (this->_listeners_[index]) {
		this->_listeners_[index](data);
		return;
	}
	
	CORE_WARN_F(CORE_LISTENER_NOT_FOUND, index , this->dispatcher_name);
}

template<typename type> 
void dispatcher<type>::clear() NOEXP {
	this->_listeners_.clear();
	CORE_DEBUG_D("all listeners in dispatcher {} is deleted !" , this->dispatcher_name);
}

} // namespace core end
#endif 