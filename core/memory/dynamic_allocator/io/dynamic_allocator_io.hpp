#pragma once

#ifndef CORE_DYNAMIC_ALLOCATOR_IO_HPP
#define CORE_DYNAMIC_ALLOCATOR_IO_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/pair/pair.hpp"
#include "core/memory/memory_enums.hpp"

namespace core {
	DLL_API_CLASS dynamic_allocator;
	DLL_API_CLASS memory_block;
	DLL_API_CLASS memory_registry;
}

// used to request memory from dynamic_allocator
struct memory_request;
struct memory_request_2;

/*
	- memory_handle used with core::dynamic_allocator to dynamically allocate memory
	- this handle can deal with reallocation and fast deallocation and query info
*/
template<typename type> class memory_handle {
private:
	static const string _typename_ = TYPE_NAME(type);

private:
	allocator_response       _response_       = allocator_response::full;
	memory_tag               _tag_            = memory_tag::unkown;
	u8                       _block_index_    = (u8)-1;
	u32                      _register_index_ = (u32)-1;
	type*                    _ptr_            = nullptr;
	core::dynamic_allocator* _allocator_      = nullptr; // parent memory allocator for this handle

	/*
		private constructor
		only friends can use tis constructor to craft handle
	*/
	memory_handle(
		allocator_response response_ , u8 b_index_ , u32 reg_index_ , type* pointer_ , 
		memory_tag tag_ , core::dynamic_allocator* allocator_
	) NOEXP;

public:
	/*
		constructor's
	*/ 
	memory_handle() NOEXP = default;
	memory_handle(memory_handle<type> &&  handle_to_move) NOEXP; // move ownership
	memory_handle(memory_handle<type> &   handle_to_move) NOEXP; // move ownership

	/*
		operator's
	*/
	memory_handle<type>& operator= (memory_handle<type> &&  handle_to_move) NOEXP; // move ownership
	memory_handle<type>& operator= (memory_handle<type> &   handle_to_move) NOEXP; // move ownership
	memory_handle<type>& operator= (int) NOEXP; // cleanup the handle

	type* operator -> () NOEXP;
	type& operator *  () NOEXP;

	// destructor
	~memory_handle() NOEXP;

	/*
		function's
	*/ 
	INLINE allocator_response       response() NOEXP { return _response_; }
	INLINE memory_tag               tag() NOEXP { return _tag_; }
	INLINE core::dynamic_allocator* allocator() NOEXP { return _allocator_; }

private: 
	// friends who can access handle internal variables
	friend DLL_API_CLASS core::dynamic_allocator;
	friend DLL_API_CLASS core::memory_block;
	friend DLL_API_CLASS core::memory_registry;

	/*
		copying is not allowed
	*/
	memory_handle(memory_handle<type> const& handle_to_copy) NOEXP = delete; 
	memory_handle<type>& operator= (memory_handle<type> const& handle_to_copy) NOEXP = delete; 

}; // class memory_handle end


// used to request memory from dynamic_allocator
template<typename type> struct memory_request {
	u64        count;
	u64        alignement;
	memory_tag tag              DEBUG_ONLY;
	u8         block_index;
	bool       wait_for_memory_block;
	bool       try_other_memory_blocks;
};

// used to request memory from dynamic_allocator
struct memory_request_2 {
	u64        size_1;
	u64        size_2;
	u64        alignement_1;
	u64        alignement_2;
	memory_tag tag_1             DEBUG_ONLY;
	memory_tag tag_2             DEBUG_ONLY;
	u8         block_index;
	bool       wait_for_memory_block;
	bool       try_other_memory_blocks;
};

#endif