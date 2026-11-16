#pragma once

#ifndef CORE_GA_HANDLES_HPP
#define CORE_GA_HANDLES_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/pair/pair.hpp"
#include "core/memory/memory_enums.hpp"

struct memory_request_g;
DLL_API_CLASS memory_handle_g;

namespace core {
namespace global_allocator {
	DLL_API same_pair<memory_handle_g> allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP;
	DLL_API memory_handle_g allocate(memory_request_g const& request) NOEXP;
	DLL_API void deallocate(memory_handle_g& handle) NOEXP;
}
}

// used to allocate memory from global allocator core::global_allocator
struct memory_request_g {
	u64 size;
	DEBUG_ONLY subsystem_memory_tag tag;  
};


/*
	- memory handle given back by core::global_allocator
*/
DLL_API_CLASS memory_handle_g {

private:
	subsystem_memory_tag _tag_      = subsystem_memory_tag::unkown;
	u64                  _size_     = NULL;
	void*                _ptr_      = nullptr;
	allocator_response   _response_ = allocator_response::busy;

	/*
		private constructor
		only friends can use tis constructor to craft handle
	*/
	memory_handle_g(allocator_response response, u64 size, subsystem_memory_tag tag_, void* pointer) NOEXP;

public:
	/*
		constructor's
	*/ 
	memory_handle_g() NOEXP = default;
	memory_handle_g(memory_handle_g && handle_to_move) NOEXP; // move ownership
	memory_handle_g(memory_handle_g &  handle_to_move) NOEXP; // move ownership

	/*
		operator's
	*/ 
	memory_handle_g& operator=(memory_handle_g && handle_to_move) NOEXP; // move ownership
	memory_handle_g& operator=(memory_handle_g &  handle_to_move) NOEXP; // move ownership

	/*
		destructor
	*/ 
	~memory_handle_g() NOEXP;

	/*
		functions
	*/
	INLINE void* pointer() NOEXP { return _ptr_; }
	INLINE allocator_response response() NOEXP { return _response_; }
	INLINE subsystem_memory_tag tag() NOEXP { return _tag_; }
	INLINE u64 size() NOEXP { return _size_; }

private: 
	// friends who can access handle internally
	friend DLL_API same_pair<memory_handle_g> core::global_allocator::allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP;
	friend DLL_API memory_handle_g core::global_allocator::allocate(memory_request_g const& request) NOEXP;
	friend DLL_API void core::global_allocator::deallocate(memory_handle_g& handle) NOEXP;

	/*
		handle copying is not allowed 
	*/
	memory_handle_g(memory_handle_g const& handle_to_copy) NOEXP = delete; 
	memory_handle_g& operator=(memory_handle_g const& handle_to_copy) NOEXP = delete;

}; // class memory_handle_g end


#endif
