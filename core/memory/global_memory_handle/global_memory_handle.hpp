#pragma once

#ifndef GLOBAL_MEMORY_HANDLE_HPP
#define GLOBAL_MEMORY_HANDLE_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/pair/pair.hpp"
#include "core/memory/memory_enums.hpp"

// friends who can access handle private variables
#define FRIENDS_TO_MEMORY_HANDLE_G() \
		DLL_API memory_handle_g   friend core::memory::allocate(memory_request_g const& request) NOEXP; \
		DLL_API same_pair<memory_handle_g> friend core::memory::allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP; \
		DLL_API void              friend core::memory::deallocate(memory_handle_g& handle) NOEXP; \
		friend  DLL_API_CLASS     core::dynamic_allocator;\
		friend  DLL_API_CLASS     core::memory_block;\
		friend  DLL_API_CLASS     core::memory_registry;


/*
	- "global memory handle" given by "global memory allocator" --> core::memory .
	- note: used with core::memory::allocate , core::memory::deallocate .
*/
template<typename type> class memory_handle_g {
private:
	bool                 _deallocate_at_destuctor_ = true;
	subsystem_memory_tag _tag_  = subsystem_memory_tag::unkown;
	u64                  _size_ = 0;
	type*                _ptr_  = nullptr;
	allocator_response   _response_ = allocator_response::busy;

public:
	/*
		constructor's
	*/
	memory_handle_g() NOEXP = default;
	memory_handle_g(allocator_response response, u64 size, subsystem_memory_tag tag_, type* pointer, bool deallocate_at_destruction_time = true) NOEXP;
	memory_handle_g(memory_handle_g<type> &&     handle_to_move) NOEXP; // move ownership
	memory_handle_g(memory_handle_g<type> const& handle_to_move) NOEXP = delete; // copying not allowed

	/*
		operator's
	*/ 
	memory_handle_g& operator=(memory_handle_g<type> &&     handle_to_move) NOEXP; // move ownership
	memory_handle_g& operator=(memory_handle_g<type> const& handle_to_copy) NOEXP = delete; // copying not allowed	
	type* operator -> () NOEXP;
	type& operator *  () NOEXP;

	// destructor
	~memory_handle_g() NOEXP;

	/*
		functions
	*/
	INLINE type* pointer() NOEXP { return _ptr_; }
	INLINE allocator_response response() NOEXP { return _response_; }
	INLINE subsystem_memory_tag tag() NOEXP { return _tag_; }
	INLINE u64 size() NOEXP { return _size_; }


private:
	FRIENDS_TO_MEMORY_HANDLE_G();

}; // class memory_handle_g end

#endif
