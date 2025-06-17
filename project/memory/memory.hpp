#pragma once 

/*
	general purpose memory allocator + few memory functions
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "macros.hpp"
#include <stdint.h>

// macro for using default C++ allocator instead of 
#define USE_STD_ALLOCATOR

enum class MEMORY_UNIT : uint16_t {
	BYTE = 1,
	KB,
	MB,
	GB
	// no need for TB or higher :)
};

// x64 & x32 default pointer type
typedef uint64_t* ptr_64;
typedef uint32_t* ptr_32;

namespace memory {
	
	// ERR init();
	void* alloc(size_t size);
	void  free(void* pointer , bool is_array = false);

	/*
		few function for cpu memory "RAM"
	*/
	
	// to get total cpu memory/ram size
	uint64_t total_cpu_memory(
		MEMORY_UNIT return_value_unit = MEMORY_UNIT::MB
	); 

	// to get total free cpu memory/ram
	uint64_t total_free_cpu_memory(
		MEMORY_UNIT return_value_unit = MEMORY_UNIT::MB
	); 

	// get sizeof current allocated memory
	uint64_t total_allocated_cpu_memory(
		MEMORY_UNIT return_value_unit = MEMORY_UNIT::BYTE
	);

} // namespace memory 

#endif