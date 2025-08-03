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

struct registry_pair {
	void* pointer = nullptr;
	u32   size    = NULL;
};

class heap {

private:
	// heap static variables
	static const u32 minimum_heap_size_allowed = KB_TO_BYTE(64 KB);
	static const u32 maximum_heap_size_allowed = MB_TO_BYTE(512 MB);

	/*
		just disabled constructor's and operator's
	*/
	heap(heap const& other) = delete;
	heap(heap&& other) = delete;
	heap& operator = (const heap& other) = delete;

private:
	// heap type/usage
	ALLOCATION_SECTION section = ALLOCATION_SECTION::UNKOWN;
	
	// heap memory variables
	u32 max_allowed_allocations = 1000;
	u32 alloc_size = NULL;
	u32 heap_size  = NULL;

	byte* start = nullptr;
	byte* end   = nullptr;
	byte* seek  = nullptr; // last free position

	u32 registered = NULL;
	u32 alloc_list_size = NULL;
	registry_pair* alloc_list = nullptr; // chaining hash table
	u32 free_list_range = 1;
	registry_pair* free_list  = nullptr; // sorted list

public:
	// constructor / destructor
	 heap(
		 u32 heap_size, 
		 u32 max_allocation = 1000,
		 ALLOCATION_SECTION heap_usage = ALLOCATION_SECTION::UNKOWN
	 );
	~heap();

	// heap public functions
	void* allocate(u32 size);
	void  deallocate(void* pointer);

	u32 size() noexcept;
	f32 size(MEMORY_UNIT return_value_unit) noexcept;

	u32 allocated(MEMORY_UNIT return_value_unit) noexcept;
	u32 available(MEMORY_UNIT return_value_unit) noexcept;

private:
	// heap private functions
	inline void merge_free_areas();
	u32 hash_pointer(void* pointer);

	inline void register_allocation(void* pointer , u32 size);
	inline void allocate_from_free_list(void** pointer, u32 size , u32 index);
	inline void unregister_allocation(void* pointer);
	// this function search the free_list looking for empty spot
	inline void find_free_location(u32& index_output , u32 _size);

	// sort from bigger to smaller
	inline void sort_list_by_size(registry_pair* list, u32 size);
	inline void sort_list_by_address(registry_pair* list, u32 size);
	inline void init_registry_list(registry_pair* list, u32 size);
};


#endif