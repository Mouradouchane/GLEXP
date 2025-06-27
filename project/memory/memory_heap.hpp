/*
	memory heap :
	- linear allocation
	- support big size allocations
	- multi memory pages with differnet sizes
*/
#pragma once

#ifndef MEMORY_HEAP_HPP
#define MEMORY_HEAP_HPP

#include <vector>
#include <unordered_map>
#include "memory.hpp"

class heap {

private:
	// just to know what section this heap is allocated for :)
	ALLOC_SECTION section = ALLOC_SECTION::UNKOWN;

	memory_page page;
	std::unordered_map<void*, uint32_t> alloc_list;
	std::unordered_map<void*, uint32_t> free_list;

public:
	// constructor / destructor
	 heap(
		 uint32_t size_in_kb , 
		 uint32_t max_allocation , 
		 ALLOC_SECTION heap_usage_type = ALLOC_SECTION::UNKOWN
	 );
	~heap();

	// heap public functions
	void* alloc(uint32_t size);
	void  free(void* pointer);

	uint64_t total_size(MEMORY_UNIT return_value_unit);
	uint64_t allocated_size(MEMORY_UNIT return_value_unit);
	uint64_t available_size(MEMORY_UNIT return_value_unit);
};


#endif