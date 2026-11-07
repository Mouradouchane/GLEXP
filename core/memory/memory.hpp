#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_HPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"
#include "core/pair/pair.hpp"

#include "memory_enums.hpp"
#include "core/memory/global_memory_handle/global_memory_handle.hpp"

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

#define MAX_MEMORY_TAGS 255
#define MAX_ALLOCATIONS_PRE_BLOCK 1024

struct memory_request;   // for specific allocator
struct memory_request_g; // for global allocator

namespace core {
	
	DLL_API_CLASS dynamic_allocator;
	DLL_API_CLASS memory_block;
	DLL_API_CLASS memory_registry;

	/*
		core::memory is a global allocator , it's just a wrapper used by other allocators like: pool, arena , ...
		with tagging system for memory debugging and monitoring .
	*/
	namespace memory {

		DLL_API void init() NOEXP;

		template<typename type> memory_handle_g<type> allocate(memory_request_g const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , ... .
		DLL_API same_pair<memory_handle_g> allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP;

		DLL_API void deallocate(memory_handle_g& handle) NOEXP;

		DLL_API u64 total_memory_usage() NOEXP;
		DLL_API u64 current_memory_usage(subsystem_memory_tag section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end

	/*
		to_string functions to convert memory tags and units to strings
	*/
	DLL_API string to_string(memory_tag _tag_) NOEXP;
	DLL_API string to_string(subsystem_memory_tag section_tag) NOEXP;
	DLL_API string bytes_to_string(u64 bytes_count) NOEXP;
} 
// namespace core end


// used by memory allocator
struct memory_request_g {
	u64 size;
	DEBUG_ONLY subsystem_memory_tag tag;
};

// used for dynamic allocator
struct memory_request {
	u64        size;
	u64        alignement;
	memory_tag tag             DEBUG_ONLY;
	u8         block_index;
	bool       wait_for_memory_block;
	bool       try_other_memory_blocks; 
};

// used for dynamic allocator
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
