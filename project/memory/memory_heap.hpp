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

class heap {

private:
	static const u32 minimum_heap_size_allowed = KB_TO_BYTE(64 KB);
	static const u32 maximum_heap_size_allowed = MB_TO_BYTE(512 MB);

private:
	// heap type/usage
	ALLOCATION_SECTION section = ALLOCATION_SECTION::UNKOWN;
	
	// heap memory variables
	u32 alloc_size = NULL; // sizeof allocated 
	u32 heap_size  = NULL; // heap size

	byte* start  = nullptr; // heap start
	byte* end    = nullptr; // heap end
	byte* seek   = nullptr; // last free position

	/* 
	// for keep-track with allocated/deallocated chunks
	// TODO: correct setup for alloc/free maps
	hash_map<byte*,u32> alloc_list; 
	hash_map<byte*,u32> free_list;  
	*/

private:
	/*
		just disabled constructor's and operator's
	*/
	heap(heap const& other) = delete;
	heap(heap&& other) = delete;
	heap& operator=(const heap& other) = delete;

public:
	// constructor / destructor
	 heap(
		 u32 size, 
		 u32 max_allocation = 1000,
		 ALLOCATION_SECTION heap_usage = ALLOCATION_SECTION::UNKOWN
	 );
	~heap();

	// heap public functions
	void* allocate(u32 size) const;
	void  deallocate(void* pointer) const;

	u32 size() noexcept;
	f32 size(MEMORY_UNIT return_value_unit) noexcept;

	u32 allocated(MEMORY_UNIT return_value_unit) noexcept;
	u32 available(MEMORY_UNIT return_value_unit) noexcept;

private:
	// heap private functions
	void merge_free_areas();
};


#endif