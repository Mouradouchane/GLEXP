#pragma once 

#ifndef CORE_GLOBAL_ARRAY_IMPL_HPP
#define CORE_GLOBAL_ARRAY_IMPL_HPP

#include "core/logger/logger.hpp"
#include "global_array.hpp"

#ifdef DEBUG
	static inline auto _carr_hpp_lgr_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static inline auto _carr_hpp_lgr_ = nullptr;
#endif

namespace core {

/*
	constructor's
*/
template<typename type> 
global_array<type>::global_array(const u32 elements_count, subsystem_memory_tag tag) NOEXP {	
	_count_ = elements_count;
	_size_  = sizeof(type) * _count_;

	_handle_ = core::memory::allocate( memory_request_g{ .size = _size_ , .tag = tag } );

	if (_handle_.response() != allocator_response::success){
		CORE_ERROR_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace ,"core::global_array self assing detected !");
		return;
	}

	_begin_ = (type*)_handle_.pointer();
}

template<typename type> // copy constructor
global_array<type>::global_array(global_array<type> const& array_to_copy) NOEXP {

	_count_ = array_to_copy._count_;
	_size_  = sizeof(type) * _count_;

	_handle_ = core::memory::allocate( memory_request_g{ .size = _size_ , .tag = array_to_copy._handle_.tag() } );
	
	if (_handle_.response() != allocator_response::success){
		CORE_ERROR_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace,"core::global_array failed to allocate memory !");
		return;
	}
	
	_begin_ = (type*)_handle_.pointer();
	
	// note: your "type" should support copying via "copy constructor"
	for(u32 i = 0; i < array_to_copy._count_; i++ ) {
		new (_begin_ + i) (array_to_copy[i]);
	}

}

template<typename type> // move constructor
global_array<type>::global_array(global_array<type>&& array_to_move) NOEXP {
	
	// move variables
	_handle_ = array_to_move._handle_;
	_begin_  = array_to_move._begin_;
	_count_  = array_to_move._count_;
	_size_   = sizeof(type) * _count_;

	array_to_move._handle_ = memory_handle_g{0};
	array_to_move._begin_ = nullptr;

}


/*
	destructor
*/
template<typename type> 
global_array<type>::~global_array() {
	if (_begin_) {
		core::memory::deallocate(_handle_);
		_begin_= nullptr;
	}
}


/*
	operator's
*/

template<typename type> // copy operator
type& global_array<type>::operator=(global_array<type> const& array_to_copy) NOEXP {

	// avoid self copy	
	if(this == &array_to_copy){
		CORE_WARN_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace, "core::global_array self copy not allowed !");
		return *this;
	}

	_count_ = array_to_copy._count_;
	_size_  = sizeof(type) * _count_;

	_handle_ = core::memory::allocate( memory_request_g{ .size = _size_ , .tag = array_to_copy._handle_.tag() } );
	
	if (_handle_.response() != allocator_response::success){
		CORE_ERROR_HPP("core::global_array failed to allocate memory !");
		return;
	}
	
	_begin_ = (type*)_handle_.pointer();
	
	// note: your "type" should support copying via "copy constructor"
	for(u32 i = 0; i < array_to_copy._count_; i++ ) {
		new (_begin_ + i) (array_to_copy[i]);
	}
}

template<typename type> // move operator
type& global_array<type>::operator=(global_array<type>&& array_to_move) NOEXP {

	// avoid self assing
	if(this == &array_to_move){
		CORE_WARN_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace, "core::global_array self moving not allowed !");
		return *this;
	}

	// move variables
	_handle_ = array_to_move._handle_;
	_begin_  = array_to_move._begin_;
	_count_  = array_to_move._count_;
	_size_   = sizeof(type) * _count_;

	array_to_move._handle_ = memory_handle_g{0};
	array_to_move._begin_ = nullptr;

	return *this;
}

template<typename type> 
type& global_array<type>::operator[] (u32 index) NOEXP {
#ifdef DEBUG
	if (index < _count_ && _begin_) return *(this->_begin_ + index);
	else {
		if (index < _count_){
			CORE_ERROR_HPP(_carr_hpp_lgr_, CORE_LOG_CONFIG_ALL, "global_array index {} out of range {} !", _index_ , _count_);
		}
		else{
			CORE_FATAL_HPP(_carr_hpp_lgr_, CORE_LOG_CONFIG_ALL , "core::global_array : attempt to access nullptr memory !");
			CORE_WARN_HPP (_carr_hpp_lgr_, CORE_LOG_CONFIG_ALL , CORE_WARNINIG_RUNTIME_CRASH);
		}
		return *(this->_end_);
	}
#else
	return *(this->_begin_ + index);
#endif
}	


/*
	functions
*/

template<typename type> u32  global_array<type>::capacity() NOEXP { return _count_; }
template<typename type> u64  global_array<type>::size()     NOEXP { return _size_; }

template<typename type> type* global_array<type>::begin()   NOEXP { return _begin_; }
template<typename type> type* global_array<type>::end()     NOEXP { return (_begin_ + _count_); }

template<typename type> subsystem_memory_tag global_array<type>::tag() NOEXP { return _handle_.tag(); }

} // namespace core end

#endif