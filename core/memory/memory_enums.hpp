#pragma once

#ifndef CORE_MEMORY_ENUMS_HPP
#define CORE_MEMORY_ENUMS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"


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

// returned by the allocators to tell you about the status of thier response !
enum class allocator_response : u8 {
	success = 0,
	busy, // the allocator is busy handling other allocation
	full, // the allocator is full and no memory left
	register_full,
	fragmeneted, // the allocator have the memory asked for but it too fragmeneted
};

/*
	"debug-only" , use it to flag allocator's 
*/
enum class subsystem_memory_tag : u8 {
	unkown = 0,
	time_system,
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
	"debug-only" , use it to flag the memory allocations
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


/*
	todo: move this structs to the right place
*/

// used by memory_block allocator
struct memory_allocation {
	void*      ptr;
	u64        size;
	// u16        alignement;
	memory_tag tag;  DEBUG_ONLY
	
	// helper function
	void clear() NOEXP {
		ptr  = nullptr;
		size = 0;
	#ifdef DEBUG
		tag = memory_tag::unkown;
	#endif
	}
};


// used by registry and allocator
struct memory_allocation_info {
	void*      ptr;
	u64        size;
	u32	       index;
	memory_tag tag;   DEBUG_ONLY
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


#endif