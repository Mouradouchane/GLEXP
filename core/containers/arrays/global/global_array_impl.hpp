#pragma once 

#ifndef CORE_GLOBAL_ARRAY_IMPL_HPP
#define CORE_GLOBAL_ARRAY_IMPL_HPP

#include "global_array.hpp"

namespace core {

/*
	constructor's
*/
template<typename type> 
global_array<type>::global_array(const u32 elements_count, subsystem_memory_tag tag) NOEXP {
			
	this->_count_ = elements_count;
	this->_size_  = sizeof(type) * _count_;

	_handle_ = core::memory::allocate(
		memory_request_g{ .size = this->_size_ , .tag = tag }
	);

	if (_handle_.response() != allocator_response::success) return;

	_begin_ = (type*)_handle_.pointer();
	_end_   = _begin_ + _count_;
}

template<typename type> 
global_array<type>::global_array(global_array<type> const& array_to_copy) NOEXP {

}

template<typename type> 
global_array<type>::global_array(global_array<type>&& array_to_move) NOEXP {

}

template<typename type> 
template<typename... args> 
global_array<type>::global_array(const u32 elements_count, subsystem_memory_tag tag, args&&... arguments) NOEXP {

}

/*
	destructor
*/
template<typename type> 
global_array<type>::~global_array() {
	if (_begin_) {
		core::memory::deallocate(this->_handle_);
		_begin_= nullptr;
	}
}

/*
	operator's
*/

template<typename type> 
type& global_array<type>::operator=(global_array<type> const& array_to_copy) NOEXP {

}

template<typename type> 
type& global_array<type>::operator=(global_array<type>&& array_to_move) NOEXP {

}

template<typename type> 
type& global_array<type>::operator[] (u32 index) NOEXP {
#ifdef DEBUG
	if (index < _count_ && _begin_) return *(this->_begin_ + index);
	else {
		if (index < _count_){
			CORE_ERROR_HPP(_carr_hpp_lgr_, 0, "global_array index {} out of range {} !", _index_ , _count_);
		}
		else{
			CORE_FATAL_HPP(_carr_hpp_lgr_, 0 , "core::global_array : attempt to access nullptr memory !");
			CORE_WARN_HPP (_carr_hpp_lgr_, 0 , CORE_WARNINIG_RUNTIME_CRASH);
		}
		return *(this->_end_);
	}
#else
	return *(this->_begin_ + index);
#endif
}	

} // namespace core end

#endif