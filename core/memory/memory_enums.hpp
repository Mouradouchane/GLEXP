#pragma once

#ifndef CORE_MEMORY_ENUMS_HPP
#define CORE_MEMORY_ENUMS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

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

// used by block allocator
struct memory_allocation {
	void* ptr;
	u32   size;

	DEBUG_ONLY u8 tag;

	// helper function
	void clear() NOEXP {
		this->ptr = nullptr;
		this->size = 0;
	#ifdef DEBUG
		this->tag = (u8)memory_tag::unkown;
	#endif
	}
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


#endif