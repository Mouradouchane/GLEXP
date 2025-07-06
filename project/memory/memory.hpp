#pragma once 

/*
	- our main "general purpose" memory allocator
	- used by other custom allocators => heap , pool , arena , ...
	- few memory functions
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <string>
#include "macros.hpp"
#include "types.hpp"

enum class allocation_section : uint8_t {
	UNKOWN    = 0,
	GENERAL   = 1,
	MODELS    = 2,
	TEXTURES  = 3,
	AUDIOS    = 4,
	ANIMATION = 5,
	PHYSICS   = 6,
};

enum class memory_unit : u8 {
	BYTE = 0,  KB = 1,  MB = 2,  GB = 3
};

struct memory_info {
	u64 size = NULL;
	u64 free = NULL;
};

// convert "KB,MB,GB" to "BYTES" macros
#define KB_TO_BYTE(KB) uint64_t(KB) * 1024
#define MB_TO_BYTE(MB) uint64_t(MB) * 1048576
#define GB_TO_BYTE(GB) uint64_t(GB) * 1073741824

// convert "BYTES" to "KB,MB,GB" macros
#define BYTE_TO_KB(BYTES) (BYTES/1024.0f)
#define BYTE_TO_MB(BYTES) (BYTES/1048576.0f)
#define BYTE_TO_GB(BYTES) (BYTES/1073741824.0f)

std::string pointer_to_hex_string(ptr64 pointer);

namespace memory {
	
	void* alloc(size_t size, allocation_section section = allocation_section::UNKOWN);
	void  free(void* pointer);

	uint64_t total_size() noexcept;
	double   total_size_f(memory_unit unit) noexcept;
	uint64_t sizeof_section(allocation_section section) noexcept;
	double   sizeof_section_f(allocation_section section , memory_unit unit) noexcept;

	// todo : move it away from namespace memory
	// to query RAM information
	memory_info get_cpu_memory_info();

} // namespace memory end

#endif