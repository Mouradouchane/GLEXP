#pragma once

#ifndef CORE_MEMORY_IO_CPP
#define CORE_MEMORY_IO_CPP

#include "core/logger/logger.hpp"
#include "global_allocator_io.hpp"

#ifdef DEBUG
static std::shared_ptr<spdlog::logger> _core_global_alloc_handle_logger_ = CORE_GET_LOGGER( MEMORY_ALLOCATOR_LOGGER );
#else
static std::shared_ptr<spdlog::logger> _core_global_alloc_handle_logger_ = nullptr;
#endif

#define _LOGGER_ _core_global_alloc_handle_logger_ 

/*
    constructor's
*/

memory_handle_g::memory_handle_g(
	allocator_response response_, u64 size, subsystem_memory_tag tag_, void* pointer
) NOEXP
	: _response_(response_), _size_(size), _tag_(tag_), _ptr_(pointer)
{

}

memory_handle_g::memory_handle_g(memory_handle_g&& handle_to_move) NOEXP {
	  
	// clean up if memory allocated
    if (_ptr_) {
        core::global_allocator::deallocate(*this);
    }

	// move
    _tag_      = handle_to_move._tag_;
    _size_     = handle_to_move._size_;
    _ptr_      = handle_to_move._ptr_;
    _response_ = handle_to_move._response_;

    // clean other
    handle_to_move._ptr_ = nullptr;

}

memory_handle_g::memory_handle_g(memory_handle_g& handle_to_move) NOEXP {
	  
	// clean up if memory allocated
    if (_ptr_) {
        core::global_allocator::deallocate(*this);
    }

	// move
    _tag_      = handle_to_move._tag_;
    _size_     = handle_to_move._size_;
    _ptr_      = handle_to_move._ptr_;
    _response_ = handle_to_move._response_;

    // clean passed handle
    handle_to_move._ptr_ = nullptr;

}

/*
    operators
*/
memory_handle_g& memory_handle_g::operator=(memory_handle_g&& handle_to_move) NOEXP {

	if (this != &handle_to_move) {
        
		// clean up if memory allocated
        if (_ptr_) {
            core::global_allocator::deallocate(*this);
        }

		// move ownership
        _tag_      = handle_to_move._tag_;
        _size_     = handle_to_move._size_;
        _ptr_      = handle_to_move._ptr_;
        _response_ = handle_to_move._response_;

        // clean passed handle
        handle_to_move._ptr_ = nullptr;
    }

    return *this;
}

memory_handle_g& memory_handle_g::operator=(memory_handle_g& handle_to_move) NOEXP {

	if (this != &handle_to_move) {
        
		// clean up if memory allocated
        if (_ptr_) {
            core::global_allocator::deallocate(*this);
        }

		// move ownership
        _tag_      = handle_to_move._tag_;
        _size_     = handle_to_move._size_;
        _ptr_      = handle_to_move._ptr_;
        _response_ = handle_to_move._response_;

        // clean passed handle
        handle_to_move._ptr_ = nullptr;
    }

    return *this;
}

/*
    destructor
*/
memory_handle_g::~memory_handle_g() NOEXP {
	if (_ptr_) {
		core::global_allocator::deallocate(*this);
	    _ptr_ = nullptr;
	}
}


#endif