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

#define HIGH_MEMORY_USAGE_DETECTED "high memory usage detected at {} , {}bytes !"
#define INVALID_MEMORY_HANLDE      "invalid memory handle passed to {} !"
 
#define MEMORY_ORDER_RELAXE  std::memory_order_relaxed // for read-only when no cache syncing is needed
#define MEMORY_ORDER_ACQUIRE std::memory_order_acquire // when cache syncing is needed

#define MAX_MEMORY_TAGS   255
#define MAX_MEMORY_BLOCKS 255
#define MAX_ALLOCATIONS_PRE_BLOCK 1024

#define FRIENDS_TO_MEMORY_HANDLE() \
		DLL_API g_memory_handle   friend core::memory::allocate(g_memory_request const& request) NOEXP; \
		DLL_API g_memory_handle_2 friend core::memory::allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP; \
		DLL_API void              friend core::memory::deallocate(g_memory_handle const& handle) NOEXP; \
		friend  DLL_API_CLASS     core::dynamic_allocator;

struct   memory_request; // for other allocator
struct g_memory_request; // for global allocator

class    memory_handle; // memory handle contain pointer + other info for allocators internal usage
class  g_memory_handle; // for global allocator
struct   memory_handle_2; // 2 memory handles in 1 struct
struct g_memory_handle_2; // for global allocator


namespace core {
	
	DLL_API_CLASS dynamic_allocator;

	/*
		core::memory is a global allocator , it's just a wrapper used by other allocators like: pool, arena , ...
		with tagging system for memory debugging and monitoring .
	*/
	namespace memory {

		DLL_API void init() NOEXP;

		DLL_API g_memory_handle allocate(g_memory_request const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , ... .
		DLL_API g_memory_handle_2 allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP;

		DLL_API void deallocate(g_memory_handle const& handle) NOEXP;

		DLL_API u64 total_memory_usage() NOEXP;
		DLL_API u64 current_memory_usage(subsystem_memory_tag section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end

	/*
		to_string functions to convert memory tags and units to strings
	*/
	DLL_API string to_string(memory_tag tag) NOEXP;
	DLL_API string to_string(subsystem_memory_tag section_tag) NOEXP;
	DLL_API string bytes_to_string(u64 bytes_count) NOEXP;
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

		subsystem_memory_tag _tag_ = subsystem_memory_tag::unkown;
		u64   _size_ = 0;
		bool _deallocate_at_destuctor_ = false;

	public:
		void* ptr = nullptr;
		allocator_response response = allocator_response::busy;

		// constructor's
		g_memory_handle() NOEXP = default;
		g_memory_handle(
			allocator_response response, u64 size, subsystem_memory_tag tag, void* pointer, bool deallocate_at_destruction_time = false
		) NOEXP;

		// destructor
		~g_memory_handle() NOEXP;
};

/*
	memory_handle used by allocators for fast allocation/deallocation memory
*/
class memory_handle {
	private:
		FRIENDS_TO_MEMORY_HANDLE();

		// this for fast look-up and memory allocation/deallocation
		u8  _block_index_    = (u8)-1;
		u32 _register_index_ = (u32)-1;

	public:
		allocator_response response = allocator_response::full;
		void* ptr = nullptr;
		
		// constructor's
		memory_handle() NOEXP = default;
		memory_handle(allocator_response response, u8 b_index, u32 reg_index, void* pointer) NOEXP;

		// destructor
		~memory_handle() NOEXP;

		INLINE u8  block_index() NOEXP { return this->_block_index_; }
		INLINE u32 register_index() NOEXP { return this->_register_index_; }

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
	DEBUG_ONLY subsystem_memory_tag tag;
};

// used for dynamic allocator
struct memory_request {
	u64 size;      // max size below 4GB
	u16 alignement;
	DEBUG_ONLY memory_tag tag;
};

#endif
