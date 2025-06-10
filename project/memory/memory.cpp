#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "macros.hpp"
#include "memory.hpp"
#include "assert.hpp"

// total cpu memory in BYTE
static uint64_t cpu_memory = NULL;

// total allocated/used cpu memory in BYTE
static uint64_t used_cpu_memory = NULL;
// total available/free cpu memory in BYTE
static uint64_t free_cpu_memory = NULL;

/*
	memory namespace functions
*/

// todo: implement custom allocation
void* memory::alloc(size_t size) {

#ifdef USE_STD_ALLOCATOR
	void* ptr = new int8_t[size];
#else
	// custom allocator not ready
	ASSERT_EXP(0)
#endif

	if(ptr) used_cpu_memory += size;

	return ptr;
}

// todo: implement custom free/delete
void memory::free(void* pointer , bool is_array ) {

	#ifdef USE_STD_ALLOCATOR
		is_array ? delete[] pointer : delete pointer;
	#else 
		// custom allocator not ready
		ASSERT_EXP(0)
	#endif

}

/*
	few functions for cpu memory
*/

uint64_t total_cpu_memory(MEMORY_UNIT return_value_unit){

	if (cpu_memory == NULL) {
		cpu_memory = 0;
	}

}
#endif