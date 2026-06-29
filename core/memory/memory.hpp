#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_HPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/strings/string.hpp"

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

#define MEMORY_ORDER_RELAXE  std::memory_order_relaxed // for read-only when no cache syncing is needed
#define MEMORY_ORDER_ACQUIRE std::memory_order_acquire // when cache syncing is needed

#define MAX_MEMORY_TAGS   255
#define MAX_MEMORY_BLOCKS 255
#define MAX_ALLOCATIONS_PRE_BLOCK 1024

namespace core {

	enum class memory_tag     : u8;

	enum class allocator_tag  : u8;
	enum class allocator_type : u8;
	enum class allocator_response : u8;

	struct   memory_request;
	struct g_memory_request;

	struct   memory_allocation;
	struct i_memory_allocation;

	struct   free_memory;
	struct i_free_memory;

	struct memory_handle;
	struct memory_handle_2;

	/*
		core::memory have global allocator it's just a wrapper used by other allocators like: pool, arena , ...
	*/
	namespace memory {

		DLL_API void* allocate(g_memory_request const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , ... .
		DLL_API two_pointers allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP;

		DLL_API void deallocate(void* pointer) NOEXP;

		DLL_API u64 total_memory_usage() NOEXP;
		DLL_API u64 current_memory_usage(u8 section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end


	/*
		few memory types
	*/
	
	enum class allocator_response : u8 {
		success = 0,
		busy, // the allocator is busy with other thread
		full, // the allocator is full and no memory left
		register_full,
		fragmeneted, // the allocator have the memory asked for but it too fragmeneted
	};

	// returned by memory allocator
	struct memory_handle {
		// allocator response "status"
		core::allocator_response response = core::allocator_response::full; 
		void* ptr = nullptr;
		// this for fast deallocation
		u8    block_index    =  (u8)-1;
		u32   register_index = (u32)-1;
	};
	
	// returend by memory allocator for tow allocations in one handle
	struct memory_handle_2 {
		core::memory_handle handle_1;
		core::memory_handle handle_2;
	};

	// used by memory allocator
	struct g_memory_request {
		u64 size; // could be higher than 4GB
		u8  tag;  // "debug-only"
	};

	// used for dynamic allocator
	struct memory_request {
		u32 size;      // max size below 4GB
		u32 alignement; 
		u8  tag; // "debug-only"
	};
	
	// used by block allocator
	struct memory_allocation {
		void* ptr;
		u32   size;
			
	#ifdef DEBUG
		u8    tag; // "debug-only"
	#endif

		// helper function
		INLINE void clear() NOEXP;
	};

	// used by registry and allocator
	struct i_memory_allocation {
		void* ptr;
		u32   size;
		u32	  index;
	};

	// used by free list and allocator
	struct free_memory {
		void* ptr;
		u32   size;
	};

	// used by registry and allocator
	struct i_free_memory {
		void* ptr;
		u32   size;
		u32	  index;
	};

	/*
		"debug-only" , used to flag allocator used for what .
	*/
	enum class allocator_tag : u8 {
		unkown = 0,

		memory_system,
		assets_system,
		entity_system,
		events_system,
		physics_system,
		graphics_system,
		collision_system,
		animation_system,
		work_system,
		gui_system,

	#ifdef DEBUG
		debug_system
	#endif
	};

	/*
		"debug-only" , used to flag the allocation used for what
	*/
	enum class memory_tag : u8 {
		unkown = 0,
		
		general,
		registry,
		event,
		thread_worker,
		entity,
		mesh,
		node,
		gui_element,
		gui_text,
		normal,
		texture,
		material,
		physics,
		collision,
		skeleton,
		ai,
		audio,
		stdcpp,
		file,
		config,
		timer,
		debugger,

	#ifdef DEBUG
		dev,
	#endif
	};


	// used with void* allocator to tell the allocator real type
	enum class allocator_type : u8 {
		unkown = 0,
		dynamic_allocator = 1,
		pool_allocator,
		arena_allocator
	};


	/*
		to_string functions to convert memory tags to strings
	*/
	DLL_API string to_string(core::memory_tag tag) NOEXP;
	DLL_API string to_string(core::memory_tag section_tag, core::memory_tag mem_tag) NOEXP;


} // namespace core end


#endif
