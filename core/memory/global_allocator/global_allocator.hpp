#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_HPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/pair/pair.hpp"
#include "core/strings/string.hpp"
#include "core/memory/memory_enums.hpp"

struct        memory_request_g;
DLL_API_CLASS memory_handle_g;

namespace core {

	/*
		note: core::global_allocator , it just a wrapper to new/delete with keep-tracking to i/o memory.
		      also have memory tagging for debugging and monitoring .
	*/
	namespace global_allocator {

		DLL_API void init() NOEXP;

		DLL_API memory_handle_g allocate(memory_request_g const& request) NOEXP;

		// note: this function preforme tow allocation in one call but !
		//       both allocations not guarnted to be next each other in memory :)
		//       because of paging , multi-threading , etc... .
		DLL_API same_pair<memory_handle_g> allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP;
		DLL_API void deallocate(memory_handle_g& handle) NOEXP;

		DLL_API u64 current_memory_usage() NOEXP; // memory usage of all
		DLL_API u64 current_memory_usage(subsystem_memory_tag section_tag) NOEXP;
		DLL_API u64 peak_memory_usage() NOEXP;

	}
	// namespace memory end


	/*
		to_string functions to convert memory tags and units to strings
	*/
	DLL_API string to_string(memory_tag _tag_) NOEXP;
	DLL_API string to_string(subsystem_memory_tag section_tag) NOEXP;
	DLL_API string bytes_to_string(u64 bytes_count) NOEXP;


} 
// namespace core end


#endif
