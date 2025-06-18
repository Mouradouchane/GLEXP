#pragma once 

/*
	general purpose memory allocator + few memory functions
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "macros.hpp"
#include <stdint.h>

enum class MEMORY_UNIT : uint16_t {
	BYTE = 1,
	KB,
	MB,
	GB
	// no need for TB or higher :)
};

struct memory_info {
	uint64_t size = NULL;
	uint64_t free = NULL;
};

// default 32bit & 64bit pointer type
typedef uint64_t* ptr_64;
typedef uint32_t* ptr_32;

namespace memory {
	
	// ERR init();
	void* alloc(size_t size);
	void  free(void* pointer , bool is_array = false);

	// to query RAM information
	memory_info get_cpu_memory_info();

} // namespace memory

#endif