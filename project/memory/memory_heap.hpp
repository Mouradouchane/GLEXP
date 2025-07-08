/*
	memory heap :
	- linear allocation
	- support big size allocations
	- multi memory pages with differnet sizes
*/
#pragma once

#ifndef MEMORY_HEAP_HPP
#define MEMORY_HEAP_HPP

#include "types.hpp"
#include "memory.hpp"

struct register_info {
	void* pointer;
	u32   size;
};

struct registry {
	register_info* list = nullptr;
	u32 size  = NULL;
	u32 range = NULL;
};

class heap {

private:
	static const u32 minimum_heap_size_allowed = KB_TO_BYTE(1);
	static const u32 maximum_heap_size_allowed = GB_TO_BYTE(1);

private:
	// heap type/usage
	allocation_section section = allocation_section::UNKOWN;
	
	// heap memory variables
	u32 alloc = NULL; // sizeof allocated 
	u32 size  = NULL; // heap size

	byte* start  = nullptr; // heap start
	byte* end    = nullptr; // heap end
	byte* seek   = nullptr; // last free position

	bool is_lapped = false; // true when seek >= end
	bool is_locked = false; // for multi-thread
	
	registry alloc_list; // allocated chunks 
	registry free_list;  // available chunks 

public:
	// constructor / destructor
	 heap(
		 u32 size , u32 max_allocation ,
		 allocation_section heap_usage = allocation_section::UNKOWN
	 );
	~heap();

	// heap public functions
	void* allocate(u32 size);
	void  deallocate(void* pointer);

	u32 heap_size(memory_unit return_value_unit) noexcept;
	u32 allocated(memory_unit return_value_unit) noexcept;
	u32 available(memory_unit return_value_unit) noexcept;

private:
	// heap private functions
	void lock();
	void unlock();

	void merge_free_areas();

};


#endif