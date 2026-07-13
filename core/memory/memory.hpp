#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_HPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"

#include "memory_enums.hpp"

/*
	few macros for memory allocators errors and warnings
*/
#define ZERO_SIZE_MEMORY            "0 size memory allocation is not allowed ."
#define ZERO_SIZE_ALLOCATION        "0 size allocation is not allowed ."
#define TO_BIG_ALLOCATION           "allocation with size={} , is bigger than allocator size={} ."
#define NOT_ALLOWED_ALLOCATION_LIST "allocation-list size={} , is not in allowed range min={} , max={} ."
#define NOT_ALLOWED_ALLOCATOR_SIZE  "memory allocator size={} , not in allowed range min={} , max={} ."
#define MEMORY_ALLOCATOR_IS_FULL    "memory allocator with name={} reach his maximunm allocations ({}/{}) ."
#define GLOBAL_ALLOCATOR_FAILED     "global allocator failed to allocate memory size={} , tag={} ."

#define ALLOCATOR_FAILED            "memory allocator {} failed to allocate {}bytes of memory for {} usage ."
#define ALLOCATOR_FAILED_2          "memory allocator {} failed to allocate_tow {} bytes of memory for {} & {} usage ."
#define FAILED_TO_CREATE_ALLOCATOR  "failed to create memory-allocator : name={} , size={} ."
#define ALLOCATOR_SIZE_NOT_ALLOWED  "requested size {}byte for allocator name={} is not allowed : min_allowed={} , max_allowed={} ."
#define ALLOCATOR_FAILED_TO_GET_INFO "memory allocator {} failed to obtain info about his internal memory !"

#define G_HIGH_MEMORY_USAGE_DETECTED "global memory allocator reach a new peak memory usage {}bytes !"
#define G_INVALID_HANLDE "invalid g_memory_handle passed to {} !"

#define MEMORY_ORDER_RELAXE  std::memory_order_relaxed // for read-only when no cache syncing is needed
#define MEMORY_ORDER_ACQUIRE std::memory_order_acquire // when cache syncing is needed

#define MAX_MEMORY_TAGS   255
#define MAX_MEMORY_BLOCKS 255
#define MAX_ALLOCATIONS_PRE_BLOCK 1024

#define FRIENDS_TO_MEMORY_HANDLE() \
		DLL_API g_memory_handle   friend core::memory::allocate(g_memory_request const& request) NOEXP; \
		DLL_API g_memory_handle_2 friend core::memory::allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP; \
		DLL_API void friend core::memory::deallocate(g_memory_handle const& handle) NOEXP;

struct   memory_request; // for other allocator
struct g_memory_request; // for global allocator

class   memory_handle; // memory handle contain pointer + other info for allocators internal usage
class g_memory_handle; // for global allocator
class   memory_handle_2; // 2 memory handles in 1 struct
class g_memory_handle_2; // for global allocator

namespace core {

	/*
		core::memory is a global allocator , it's just a wrapper used by other allocators like: pool, arena , ...
		with tagging system for memory debugging and monitoring .
	*/
	namespace memory {

		DLL_API g_memory_handle allocate(g_memory_request const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , ... .
		DLL_API g_memory_handle_2 allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP;

		DLL_API void deallocate(g_memory_handle const& handle) NOEXP;

		DLL_API u64 total_memory_usage() NOEXP;
		DLL_API u64 current_memory_usage(allocator_tag section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end

	/*
		to_string functions to convert memory tags to strings
	*/
	DLL_API string to_string(memory_tag tag) NOEXP;
	DLL_API string to_string(allocator_tag section_tag) NOEXP;

} 
// namespace core end


/*
	memory handles
*/

/*
	- memory handle but for global allocator .
	- used by allocators to allocate thier own memory to manange .
*/
class g_memory_handle {
	private:
		FRIENDS_TO_MEMORY_HANDLE();

		allocator_response _response_ = allocator_response::busy;
		allocator_tag      _tag_ = allocator_tag::unkown;
		u64   _size_ = 0;
		void* _ptr_ = nullptr;

	public:
		// constructor's
		g_memory_handle() NOEXP = default;
		g_memory_handle(allocator_response response, u64 size, allocator_tag tag, void* pointer) NOEXP;

		// destructor
		~g_memory_handle() NOEXP;

		// handle functions
		allocator_response response() NOEXP;
		void* get_pointer() NOEXP;
};

/*
	memory_handle used by allocators for fast allocation/deallocation memory
*/
class memory_handle {
	private:
		FRIENDS_TO_MEMORY_HANDLE();

		allocator_response _response_ = allocator_response::full;

		// this for fast look-up and memory allocation/deallocation
		u8  _block_index_    = (u8)-1;
		u32 _register_index_ = (u32)-1;

		void* _ptr_ = nullptr;

	public:
		// constructor's
		memory_handle() NOEXP = default;
		memory_handle(allocator_response response, u8 b_index, u32 reg_index, void* pointer) NOEXP;

		// destructor
		~memory_handle() NOEXP;

		// handle functions
		allocator_response response() NOEXP;
		void* get_pointer() NOEXP;
};

// returend by memory allocator for tow allocations in one handle
struct memory_handle_2 {
	memory_handle handle_1;
	memory_handle handle_2;
};

struct g_memory_handle_2 {
	g_memory_handle handle_1;
	g_memory_handle handle_2;
};


// used by memory allocator
struct g_memory_request {
	u64 size; // could be higher than 4GB
	DEBUG_ONLY allocator_tag tag;
};

// used for dynamic allocator
struct memory_request {
	u64 size;      // max size below 4GB
	u16 alignement;
	DEBUG_ONLY memory_tag tag;
};

#endif
