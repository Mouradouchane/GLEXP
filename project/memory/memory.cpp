#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "macros.hpp"
#include "memory.hpp"
#include "assert.hpp"

#define HWINFO_IMPORTS
#include "hwinfo/ram.h"

// total allocated/used cpu memory in BYTE
static uint64_t allocated_size = NULL;

/*
	memory namespace functions
*/

// todo: implement custom allocation
void* memory::alloc(size_t size) {
	
	// todo : 0byte allocation crash

	void* pointer = new int8_t[size];
	if(pointer) allocated_size += size;

	return pointer;
}

// todo: implement custom free/delete
void memory::free(void* pointer , bool is_array ) {

	#ifdef USE_STD_ALLOCATOR
	if (pointer) allocated_size -= 0;
		is_array ? delete[] pointer : delete pointer;
	#else 
		// custom allocator not ready
		ASSERT_EXP(0)
	#endif

}

/*
	few functions for cpu memory
*/

uint64_t memory::ram_size( ){

	hwinfo::Memory mem = hwinfo::Memory();
	std::vector<hwinfo::Memory::Module> mdls = mem.modules();

	int64_t t = mem.total_Bytes();
	int64_t f = mem.free_Bytes();
	int64_t a = mem.available_Bytes();

	return uint64_t( mem.total_Bytes() );
}

uint64_t memory::free_ram( ) {
	hwinfo::Memory mem;
	return uint64_t( mem.free_Bytes() );;
}
#endif