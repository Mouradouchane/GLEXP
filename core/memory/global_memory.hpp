#pragma once 

#ifndef CORE_GLOBAL_MEMORY_HPP
#define CORE_GLOBAL_MEMORY_HPP

#include <string>
#include "core/macros.hpp"
#include "core/types.hpp"

enum class memory_usage : u16 {
    unkown     = 0,
	general    ,
	models     ,
	textures   ,
	audios     ,
	animations ,
	physics    ,
	ai         ,
	strings    ,
	buffers    ,
	threads    ,
    stdcpp     ,
};

enum class memory_unit : u8 {
	byte = 0,  kb = 1,  mb = 2,  gb = 3
};

struct memory_info {
	u64 size = NULL;
	u64 free = NULL;
};

// example: fn(2 KB) --> fn(2 *1024)
#define KB *1024u
#define MB *1048576u
#define GB *1073741824u

// convert "kb,mb,gb" to "BYTES" macros
#define KB_TO_BYTE(_KB) u64(_KB) * 1024u
#define MB_TO_BYTE(_MB) u64(_MB) * 1048576u
#define GB_TO_BYTE(_GB) u64(_GB) * 1073741824u

// convert "BYTES" to "kb,mb,gb" macros
#define BYTE_TO_KB(_BYTE) (_BYTE / 1024.0f)
#define BYTE_TO_MB(_BYTE) (_BYTE / 1048576.0f)
#define BYTE_TO_GB(_BYTE) (_BYTE / 1073741824.0f)

// TODO: move this to string utility !!!!!!!!!!!!!!!
std::string pointer_to_hex_string(ptr64 pointer);

namespace core {

	/*
		- main "general purpose" global_memory allocator
		- used by other custom allocators => memory_heap,memory_pool,memory_arena,...
	*/
	namespace global_memory {

		void* allocate(size_t size, memory_usage section = memory_usage::unkown);
		void  deallocate(void* pointer);

		// todo: rename all those functions 
		uint64_t total_size() noexcept;
		double   total_size_f(memory_unit unit) noexcept;
		uint64_t sizeof_section(memory_usage section) noexcept;
		double   sizeof_section_f(memory_usage section, memory_unit unit) noexcept;

		// todo : move it to "system info"
		memory_info get_cpu_memory_info() noexcept;

	} // namespace global_memory end

} // core namespace end

#endif
