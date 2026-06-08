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
namespace core {

	enum class memory_tag : u8;
	enum class allocator_type : u8;

	// struct used to request memory from allocator
	struct memory_request {
		u64 size;       // memory size
		u32 alignement; // memory alignement
		u8  tag;        // memory tag for "debug-only"
	};

	// used to create memory-allocator 
	struct allocator_parameters {
		string* name;
		u64     size;
		bool    multi_threaded;
		core::memory_tag tag;
	};

	/*
		core::memory have global allocator it's just a wrapper used by other allocators like: pool, arena , ...
	*/
	namespace memory {

		DLL_API void* allocate(memory_request const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , ... .
		DLL_API tow_pointers allocate_tow(memory_request const& request_1, memory_request const& request_2) NOEXP;

		DLL_API void deallocate(void* pointer) NOEXP;

		DLL_API u64 current_memory_usage() NOEXP;
		DLL_API u64 current_memory_usage(core::memory_tag section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end

	// convert memory tags to strings 
	DLL_API const std::string& to_string(core::memory_tag tag) NOEXP;
	DLL_API const std::string& to_string(core::memory_tag section , u8 tag) NOEXP;

	
	/*
		debug-only option , used to flag the allocation used for what
	*/
	enum class memory_tag : u8 {
		unkown = 0,

		general,
		memory,
		event,
		work_system,
		entity_system,
		assets,
		gui,
		graphics,
		physics,
		collision,
		animations,
		ai,
		audio,
		stdcpp,
		core,
		files,
		configs,

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


} // namespace core end


#endif
