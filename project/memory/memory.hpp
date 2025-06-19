#pragma once 

/*
	general purpose memory allocator + few memory functions
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "macros.hpp"
#include <string>
#include <stdint.h>

enum class MEMORY_UNIT : uint16_t {
	BYTE = 1,
	KB   = 2,
	MB   = 3,
	GB   = 4
	// no need for TB or higher :)
};

struct memory_info {
	uint64_t size = NULL;
	uint64_t free = NULL;
};

// default 32bit & 64bit pointer type
typedef uint64_t* ptr_64;
typedef uint32_t* ptr_32;
	
// convert to byte functions
uint64_t kb_to_byte(size_t KB) noexcept;
uint64_t mb_to_byte(size_t MB) noexcept;
uint64_t gb_to_byte(size_t GB) noexcept;
// to convert byte values
double byte_to_kb(size_t byte) noexcept;
double byte_to_mb(size_t byte) noexcept;
double byte_to_gb(size_t byte) noexcept;

std::string pointer_to_hex_string(ptr_64 pointer);

namespace memory {
	
	void* alloc(size_t size);
	void  free(void* pointer);

	uint64_t allocated_memory_size();

	// to query RAM information
	memory_info get_cpu_memory_info();

} // namespace memory

#endif