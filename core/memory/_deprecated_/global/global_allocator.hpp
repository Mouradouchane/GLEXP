#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_HPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_HPP

#include "core/memory/memory_macros.hpp"
#include "core/memory/memory_enums.hpp"

namespace core {

	// struct used to request memory from allocator
	struct memory_request {
		u64 size;             // memory size
		u32 alignement;       // memory alignement
		core::memory_tag tag; // memory tag "debug-only"
	};

	/*
		core::memory have global allocator it's just a wrapper used by other allocators like: pool, arena , ...
	*/
	namespace memory {

		DLL_API void* allocate(memory_request const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , ... .
		DLL_API tow_pointers allocate_tow(memory_request const& request_1 , memory_request const& request_2) NOEXP;

		DLL_API void deallocate(void* pointer) NOEXP;

		DLL_API u64 current_memory_usage() NOEXP;
		DLL_API u64 current_memory_usage(core::memory_tag section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end
	
	// convert memory tags to strings 
	DLL_API std::string to_string(core::memory_tag tag) NOEXP;

} // namespace core end


#endif
