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
#include "hash_map.hpp"
#include "memory.hpp"

class heap {

private:
	static const u32 minimum_heap_size_allowed = KB_TO_BYTE(1);
	static const u32 maximum_heap_size_allowed = GB_TO_BYTE(1);

private:
	// heap type/usage
	ALLOCATION_SECTION section = ALLOCATION_SECTION::UNKOWN;
	
	// heap memory variables
	u32 alloc = NULL; // sizeof allocated 
	u32 size  = NULL; // heap size

	byte* start  = nullptr; // heap start
	byte* end    = nullptr; // heap end
	byte* seek   = nullptr; // last free position

	bool is_locked = false; // for multi-thread
	
	/* 
	// for keep-track with allocated/deallocated chunks
	TODO: correct setup for alloc/free maps
	hash_map<byte*,u32> alloc_list; 
	hash_map<byte*,u32> free_list;  
	*/

public:
	// constructor / destructor
	 heap(
		 u32 size , u32 max_allocation ,
		 ALLOCATION_SECTION heap_usage = ALLOCATION_SECTION::UNKOWN
	 );
	~heap();

	// heap public functions
	void* allocate(u32 size);
	void  deallocate(void* pointer);

	u32 heap_size(MEMORY_UNIT return_value_unit) noexcept;
	u32 allocated(MEMORY_UNIT return_value_unit) noexcept;
	u32 available(MEMORY_UNIT return_value_unit) noexcept;

private:
	// heap private functions
	void lock();
	void unlock();

	void merge_free_areas();

};


#endif