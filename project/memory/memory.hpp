#pragma once 

/*
	- our main "general purpose" memory allocator
	- used by other custom allocators => heap , pool , arena , ...
	- few memory functions
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "macros.hpp"
#include <string>
#include <stdint.h>

enum class ALLOC_SECTION : uint8_t {
	UNKOWN          = 0,
	GENERAL_PURPOSE = 1,
	MODELS          = 2,
	TEXTURES        = 3,
	AUDIOS          = 4,
	ANIMATION       = 5,
	PHYSICS         = 6,
};

enum class MEMORY_UNIT : uint8_t {
	BYTE = 0,  KB = 1,  MB = 2,  GB = 3
};

// few typedefs for usage :)
typedef void*     ptr;
typedef uint8_t*  ptr8;
typedef uint16_t* ptr16;
typedef uint32_t* ptr32;
typedef uint64_t* ptr64;

struct memory_info {
	uint64_t size = NULL;
	uint64_t free = NULL;
};

struct memory_page {
	uint32_t allocated = NULL; // allocated size
	uint32_t free      = NULL; // free size
	uint32_t size      = NULL; // memory size
	void*    start     = nullptr; // memory start 
	void*    end       = nullptr; // memory end 
	void*    seek      = nullptr; // last free position
	bool     lapped    = false; // true when seek >= end
	bool     locked    = false; // lock/unlock for multi-threading
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

	void* alloc(size_t size, ALLOC_SECTION section = ALLOC_SECTION::UNKOWN);
	void  free(void* pointer);

	uint64_t total_size() noexcept;
	double   total_size_f(MEMORY_UNIT unit) noexcept;
	uint64_t sizeof_section(ALLOC_SECTION section) noexcept;
	double   sizeof_section_f(ALLOC_SECTION section , MEMORY_UNIT unit) noexcept;

	// todo : move it away from namespace memory
	// to query RAM information
	memory_info get_cpu_memory_info();

} // namespace memory end

#endif