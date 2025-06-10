#pragma once 

/*
	general purpose memory allocator + few memory functions
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <stdint.h>

enum class MEMORY_UNIT : uint16_t {
	BYTE = 1,
	KB,
	MB,
	GB
	// no need for TB or higher :)
};

// macro for using default C++ allocator instead of 
#define USE_STD_ALLOCATOR

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