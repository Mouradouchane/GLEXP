#pragma once 

#ifndef CORE_GLOBAL_MEMORY_CPP
#define CORE_GLOBAL_MEMORY_CPP

#include "core/macros.hpp"

#ifdef WINDOWS
	// note: windef's to avoid conflict with other type like byte,bool,...
	#ifndef __wtypes_h__
		#include <wtypes.h>
	#endif

	#ifndef __WINDEF_
		#include <windef.h>
	#endif

	#include <Windows.h>
#endif

#include <array>
#include <sstream>
#include <map>

#include "core/assert.hpp"
#include "core/status/status.hpp"
#include "core/logger/logger.hpp"


typedef struct alloc_info {
	size_t count;
	u8     memory_section;
};

// here is how much memory is allocated by each section
// note: look at core::memory::section !
static std::array<u64, 32> sections_sizes = { 0u };

namespace core {

	namespace memory {

	// global memory-allocator variables
	static u64 allocated_size = NULL;
	static std::map < ptr, alloc_info > allocations_list = {};
	// =========================================

		DLL_API u64 sizeof_section(core::memory::section section) noexcept {
			u8 _section = (u8)section;

			if (_section < sections_sizes.size()) {
				return sections_sizes[_section];
			}
			else return NULL;
		}

		DLL_API u64 sizeof_allocated() noexcept {
			return allocated_size;
		}

		DLL_API void* allocate(size_t count, core::memory::section section) noexcept {

			CRASH_IF(count < 1, "allocate zero count allocation not allowed !")

			void* pointer = new byte[count];

			CRASH_IF((pointer == nullptr), "allocate failed to allocate global_memory");

			// update total count
			allocated_size += count;

			// update section count
			u8 _section = (u8)section;
			if (_section < sections_sizes.size()) {
				sections_sizes[_section] += count;
			}
			else sections_sizes[0] += count;

			// update allocate list
			allocations_list.insert({
				(ptr)pointer , alloc_info{count , _section}
			});

			CORE_WARN(status::get_warning(core::warning::allocated_with_global_memory), count);

			return pointer;
		}

		DLL_API void deallocate(void* pointer) noexcept {

			CRASH_IF(pointer == nullptr, "illegal deallocate null-pointer !");

			// check alloc_list
			auto allocation = allocations_list.find((ptr64)pointer);

			CRASH_IF(
				allocation == allocations_list.end(),
				"deallocate invalid pointer " + pointer_to_hex_string((ptr64)pointer)
			);

			// update total count
			allocated_size = (allocated_size - allocation->second.count) < 0 ? 0 : allocated_size - allocation->second.count;

			// update section count
			u8 _section = allocation->second.memory_section;
			if (_section < sections_sizes.size()) {
				sections_sizes[_section] = (sections_sizes[_section] - allocation->second.count) < 0 ? 0 : (sections_sizes[_section] - allocation->second.count);
			}
			else {
				// out of range _section mean there's a bug in allocate
				// so we need assert crash
				CORE_ASSERT(true, "allocate --> out of range _section mean there's a bug !!!");
			}

			// update allocate list
			allocations_list.erase(allocation->first);

			delete[] pointer;
		}

	} // global_allocator end

} // core end

#endif