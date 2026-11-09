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
	_capacity_ = elements_count ? elements_count : 1;
	_size_  = sizeof(type) * _capacity_;

	_handle_ = core::global_allocator::allocate( memory_request_g{ .size = _size_ , .tag = tag } );

	if (_handle_.response() != allocator_response::success){
		CORE_ERROR_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace ,"core::global_array self assing detected !");
		return;
	}

	_begin_ = (type*)_handle_.pointer();
	
	// construct elements
	type* end = _begin_ + _capacity_;
	for (type* ptr = _begin_; ptr != end; ptr++) {
		new (ptr) type();
	}

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0, "new core::global_array< {} >[{}] created for {} with size {}.", _typename_, _capacity_, core::to_string(tag), _size_);
}

template<typename type> // copy constructor
global_array<type>::global_array(global_array<type> const& array_to_copy) NOEXP {

	_capacity_ = array_to_copy._capacity_;
	_size_  = sizeof(type) * _capacity_;

	_handle_ = core::global_allocator::allocate( memory_request_g{ .size = _size_ , .tag = array_to_copy._handle_.tag() } );
	
	if (_handle_.response() != allocator_response::success){
		CORE_ERROR_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace,"core::global_array failed to allocate memory !");
		return;
	}
	
	_begin_ = (type*)_handle_.pointer();
	
	for(u32 i = 0; i < array_to_copy._capacity_; i++ ) {
		// note: your "type" should support "copy via constructor"
		new (_begin_ + i) type(array_to_copy[i]);
	}

}

template<typename type> // move constructor
global_array<type>::global_array(global_array<type>&& array_to_move) NOEXP {
	
	// move ownership
	_handle_ = array_to_move._handle_;
	_begin_  = array_to_move._begin_;
	_capacity_  = array_to_move._capacity_;
	_size_   = sizeof(type) * _capacity_;

	array_to_move._handle_ = memory_handle_g{0};
	array_to_move._begin_ = nullptr;

}


/*
	destructor
*/
template<typename type> 
global_array<type>::~global_array() {
	
	if (_begin_) {

		if constexpr (!std::is_trivially_destructible<type>::value) {
			// destruct elements
			for (type* ptr = _begin_; ptr != _end_; ptr++) {
				ptr->~type();
			}
		}

		_begin_= nullptr;
	}
	
	/*
		note: handle destructor will deallocate memory automatically
	*/ 

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0, "destructed core::local_array< {} >[{}] used for {} .", _typename_, _capacity_, core::to_string(_tag_));
}


/*
	operator's
*/

/*
	note: copying will replace current elements in the array , "this is not insert or append operation"
*/
template<typename type> // copy operator
type& global_array<type>::operator=(global_array<type> const& array_to_copy) NOEXP {

	// self assing not allowed
	if(this == &array_to_copy){
		CORE_ERROR_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace, "core::global_array self copy not allowed !");
		return *this;
	}

	// destruct/remove current elements
	if(_begin_){
		for (u64 i = 0; i < _capacity_; i++) {
			(_begin_ + i)->~type();
		} 
	}

	// resize array if needed 
	if (_capacity_ < array_to_copy._capacity_) {
		_handle_.~memory_handle_g(); 

		// update array size 
		_capacity_ = array_to_copy._capacity_;
		_size_  = sizeof(type) * _capacity_;

		_handle_ = core::global_allocator::allocate( 
			memory_request_g{ 
				.size = _size_,
				.tag  = array_to_copy._handle_.tag() 
			}
		);

		if (_handle_.response() != allocator_response::success){
			CORE_ERROR_HPP("core::global_array failed to allocate memory !");
			return;
		}
	}

	// copy elements
	_begin_ = (type*)_handle_.pointer();
	
	for(u32 i = 0; i < array_to_copy._capacity_; i++ ) {
		// note: your "type" should support "copy via constructor"
		new (_begin_ + i) type(array_to_copy[i]);
	}
}

/*
	note: moving will replace current elements in the array , "this is not insert or append operation"
*/
template<typename type> // move operator
type& global_array<type>::operator=(global_array<type>&& array_to_move) NOEXP {

	// self assing is not allowed
	if(this == &array_to_move){
		CORE_WARN_HPP(_carr_hpp_lgr_, core::logger::log_config::dump_stack_trace, "core::global_array self moving not allowed !");
		return *this;
	}

	// destruct/remove current elements
	if(_begin_){
		for (u64 i = 0; i < _capacity_; i++) {
			(_begin_ + i)->~type();
		}

		// deallocate current memory
		_handle_.~memory_handle_g();
	}

	// move ownership
	_handle_ = array_to_move._handle_;
	_begin_  = array_to_move._begin_;
	_capacity_  = array_to_move._capacity_;
	_size_   = sizeof(type) * _capacity_;

	array_to_move._handle_ = memory_handle_g{ 0 };
	array_to_move._begin_  = nullptr;

	return *this;
}

template<typename type> 
type& global_array<type>::operator[] (u32 index) NOEXP {
#ifdef DEBUG
	if (index < _capacity_ && _begin_) return *(this->_begin_ + index);
	else {
		if (index < _capacity_){
			CORE_ERROR_HPP(_carr_hpp_lgr_, CORE_LOG_CONFIG_ALL, "global_array index {} out of range {} !", _index_ , _capacity_);
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
	global array functions
*/

template<typename type> u64  global_array<type>::capacity() NOEXP { 
	return _capacity_; 
}
template<typename type> u64  global_array<type>::size()     NOEXP { 
	return _size_; 
}

template<typename type> type* global_array<type>::begin()   NOEXP { 
	return _begin_; 
}
template<typename type> type* global_array<type>::end()     NOEXP { 
	return (_begin_ + _capacity_); 
}

template<typename type> subsystem_memory_tag global_array<type>::tag() NOEXP {
	return _handle_.tag(); 
}

} // namespace core end

#endif