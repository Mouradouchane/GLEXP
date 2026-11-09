#pragma once

#ifndef CORE_DYNAMIC_ALLOCATOR_IO_IMPL_HPP
#define CORE_DYNAMIC_ALLOCATOR_IO_IMPL_HPP

#include "core/logger/logger.hpp"
#include "dynamic_allocator_io.hpp"

#ifdef DEBUG
static inline auto da_io_impl_hpp = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else
static inline auto daii_lgr = nullptr;
#endif


/*
	constructor's
*/

template<typename type> 
memory_handle<type>::memory_handle(
	allocator_response response_, u8 b_index_, u32 reg_index_, type* pointer_,
	memory_tag tag_, core::dynamic_allocator* allocator_
) NOEXP
	: _response_(response_), _block_index_(b_index_), _register_index_(reg_index_), _ptr_(pointer_) , 
	  _allocator_(allocator_) , _tag_(tag_)
{

}

/*
	destructor
*/

template<typename type> 
memory_handle<type>::~memory_handle() NOEXP {

	if (ptr) {
		_allocator_->deallocate(*this);
	}

	ptr = nullptr;
}

template<typename type> // move ownership
memory_handle<type>::memory_handle(memory_handle<type>&& handle_to_move) NOEXP {
	  
	// deallocate memory if we have
    if (_ptr_) {
       _allocator_->deallocate(*this);
    }

	// move ownership
    _response_       = handle_to_move._response_;
    _tag_            = handle_to_move._tag_;
	_block_index_    = handle_to_move._block_index_;
	_register_index_ = handle_to_move._register_index_;
    _ptr_            = handle_to_move._ptr_;
    _allocator_      = handle_to_move._allocator_;

	// cleanup passed handle
    handle_to_move = 0;

} 

template<typename type> // move ownership
memory_handle<type>::memory_handle(memory_handle<type>& handle_to_move) NOEXP {
		  
	// deallocate memory if we have
    if (_ptr_) {
       _allocator_->deallocate(*this);
    }

	// move ownership
    _response_       = handle_to_move._response_;
    _tag_            = handle_to_move._tag_;
	_block_index_    = handle_to_move._block_index_;
	_register_index_ = handle_to_move._register_index_;
    _ptr_            = handle_to_move._ptr_;
    _allocator_      = handle_to_move._allocator_;

	// cleanup passed handle
    handle_to_move = 0;

} 


/*
	operator's
*/
template<typename type> // move ownership
memory_handle<type>& memory_handle<type>::operator= (memory_handle<type>&& handle_to_move) NOEXP{
		  
	// deallocate memory if we have
    if (_ptr_) {
       _allocator_->deallocate(*this);
    }

	// move ownership
    _response_       = handle_to_move._response_;
    _tag_            = handle_to_move._tag_;
	_block_index_    = handle_to_move._block_index_;
	_register_index_ = handle_to_move._register_index_;
    _ptr_            = handle_to_move._ptr_;
    _allocator_      = handle_to_move._allocator_;

	// cleanup passed handle
    handle_to_move = 0;

}

template<typename type> // move ownership
memory_handle<type>& memory_handle<type>::operator= (memory_handle<type>& handle_to_move) NOEXP{
		  
	// deallocate memory if we have
    if (_ptr_) {
       _allocator_->deallocate(*this);
    }

	// move ownership
    _response_       = handle_to_move._response_;
    _tag_            = handle_to_move._tag_;
	_block_index_    = handle_to_move._block_index_;
	_register_index_ = handle_to_move._register_index_;
    _ptr_            = handle_to_move._ptr_;
    _allocator_      = handle_to_move._allocator_;

	// cleanup passed handle
    handle_to_move = 0;

}

template<typename type> // cleanup the handle
memory_handle<type>& memory_handle<type>::operator= (int) NOEXP {
	
	// deallocate
	if (_ptr_) {
		_allocator_->deallocate(*this);
	}

	// clean up
	_response_       = allocator_response::full;
	_tag_            = memory_tag::unkown;
	_block_index_    = (u8)-1;
	_register_index_ = (u32)-1;
	_magic_          = NULL;   
	_ptr_            = nullptr;
	_allocator_      = nullptr;

}

template<typename type> 
type* memory_handle<type>::operator -> () NOEXP {

#ifdef DEBUG
	if(_ptr_) return _ptr_;

	CORE_FATAL_HPP(
		daii_lgr,CORE_LOG_CONFIG_ALL,"memory_handle<{}>: {} attempt to access nullptr memory using 'operator ->' !" ,
		 _typename_ , core::pointer_to_hex_string(this)
	);
	CORE_WARN_HPP(daii_lgr, 0, "memory_handle<{}>: {} !", _typename_,  CORE_WARNINIG_RUNTIME_CRASH);
	return nullptr;
#else
	return _ptr_;
#endif

}

template<typename type> 
type& memory_handle<type>::operator * () NOEXP {

#ifdef DEBUG
	if(_ptr_) return *_ptr_;

	CORE_FATAL_HPP(
		daii_lgr,CORE_LOG_CONFIG_ALL,"memory_handle<{}>: {} attempt to access nullptr memory using 'operator *' !" ,
		 _typename_ , core::pointer_to_hex_string(this)
	);
	CORE_WARN_HPP(daii_lgr, 0, "memory_handle<{}>: {} !", _typename_,  CORE_WARNINIG_RUNTIME_CRASH);
	return *_ptr_;
#else
	return *_ptr_;
#endif

}


#endif
