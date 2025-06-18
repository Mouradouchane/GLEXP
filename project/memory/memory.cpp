#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "macros.hpp"
#include "assert.hpp"
#include "errors.hpp"
#include "memory.hpp"

#ifdef WINDOWS
	#include <Windows.h>
#endif
namespace memory {
	
	// total allocated/used cpu memory in BYTE
	static uint64_t allocated_size = NULL;

}

/*
	memory namespace functions
*/

// todo: implement custom allocation
void* memory::alloc(size_t size) {
	DEBUG_BREAK;
	CRASH_AT_FALSE(size , "not allowed zero byte allocation !")
	
	void* pointer = new int8_t[size];

	CRASH_AT_FALSE( (pointer != nullptr) , "failed to allocate memory");
	memory::allocated_size += size;
	
	return pointer;
}

// todo: free memory based on look_up table
void memory::free(void* pointer , bool is_array ) {
	DEBUG_BREAK;
	CRASH_AT_FALSE( (pointer != nullptr) , "free null-pointer memory not allowed");

	(is_array) ? delete[] pointer : delete pointer;

}

memory_info memory::get_cpu_memory_info() {
	DEBUG_BREAK;

#ifdef WINDOWS
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex); 

	if (GlobalMemoryStatusEx(&statex)) {
		return memory_info{
			statex.ullTotalPhys,
			statex.ullAvailPhys
		};
	}
	else return memory_info();
#else
	ASSERT_EXP(0)
#endif

}

#endif