#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_HPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_HPP

#include "core/strings/string.hpp"
#include "core/memory/registery/registery.hpp"

namespace core {

	/*
		global memory allocator it's just a wrapper for new/delete and used by other 
		allocators like: pool, arena , ...
	*/
	namespace memory {

		// convert memory tags to strings 
		DLL_API std::string tag_to_string(tag memory_tag) NOEXP;

		DLL_API void* allocate(u32 size, core::memory::tag memory_tag) NOEXP;

		// this function preforme 2 allocation in one call
		// note: both allocations not guarnted to be next each other in memory :)
		DLL_API tow_pointers allocate_tow(u32 size_1, u32 size_2, core::memory::tag memory_tag) NOEXP;

		DLL_API void deallocate(void* pointer) NOEXP;

		// return the size of total memory allocated at the momeny
		// note: size in bytes
		DLL_API u64 total_used_memory() NOEXP;
		DLL_API u64 used_memory(core::memory::tag memory_tag) NOEXP;

	}
	// namespace memory end
	

} // namespace core end


#endif
