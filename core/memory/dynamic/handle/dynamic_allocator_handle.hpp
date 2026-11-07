#pragma once

#ifndef DYNAMIC_ALLOCATOR_MEMORY_HANDLE_HPP
#define DYNAMIC_ALLOCATOR_MEMORY_HANDLE_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/pair/pair.hpp"
#include "core/memory/memory_enums.hpp"

namespace core {
	DLL_API_CLASS dynamic_allocator;
	DLL_API_CLASS memory_block;
	DLL_API_CLASS memory_registry;
}

// friends who can access handle private variables
#define FRIENDS_TO_MEMORY_HANDLE() \
		DLL_API memory_handle_g   friend core::memory::allocate(memory_request_g const& request) NOEXP; \
		DLL_API same_pair<memory_handle_g> friend core::memory::allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP; \
		DLL_API void              friend core::memory::deallocate(memory_handle_g& handle) NOEXP; \
		friend  DLL_API_CLASS     core::dynamic_allocator;\
		friend  DLL_API_CLASS     core::memory_block;\
		friend  DLL_API_CLASS     core::memory_registry;


/*
	- memory_handle used with core::dynamic_allocator to dynamically allocate memory
	- smart handle can deal with reallocation and fast deallocation/query info
*/
template<typename type> class memory_handle {
private:
	allocator_response       _response_ = allocator_response::full;
	memory_tag               _tag_      = memory_tag::unkown;
	u8                       _block_index_    = (u8)-1;
	u32                      _register_index_ = (u32)-1;
	u32                      _magic_          = NULL; // for "future" usage for reallocation
	type*                    _ptr_       = nullptr;
	core::dynamic_allocator* _allocator_ = nullptr;   // parent memory allocator for this handle

public:
	/*
		constructor's
	*/ 
	memory_handle() NOEXP = default;
	memory_handle(allocator_response response, u8 b_index, u32 reg_index, type* pointer , memory_tag tag, core::dynamic_allocator* allocator_) NOEXP;
	memory_handle(memory_handle &&     handle_to_move) NOEXP; // move ownership
	memory_handle(memory_handle const& handle_to_copy) NOEXP = delete; // copying not allowed

	/*
		operator's
	*/
	memory_handle& operator= (memory_handle &&     handle_to_move) NOEXP; // move ownership
	memory_handle& operator= (memory_handle const& handle_to_copy) NOEXP = delete; // copying not allowed
	type* operator -> () NOEXP;
	type& operator *  () NOEXP;
	
	// destructor
	~memory_handle() NOEXP;

	/*
		functions
	*/ 
	INLINE type*              pointer() NOEXP { return _ptr_; }
	INLINE u8                 block_index() NOEXP { return _block_index_; }
	INLINE u32                register_index() NOEXP { return _register_index_; }
	INLINE allocator_response response() NOEXP { return _response_; }
	INLINE memory_tag         tag() NOEXP { return _tag_; }
	INLINE core::dynamic_allocator* allocator() NOEXP { return _allocator_; }

	/*
		operators
	*/

private:
	FRIENDS_TO_MEMORY_HANDLE();

}; // class memory_handle end

#endif