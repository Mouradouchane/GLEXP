#pragma once 

#ifndef CORE_MEMORY_CPP
#define CORE_MEMORY_CPP

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

#include "memory.hpp"

#ifdef DEBUG 
	static bool logs_enabled = true;
#endif

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

		DLL_API u64 sizeof_section(core::memory::tag tag) noexcept {
			u8 _section = (u8)tag;

			if (_section < sections_sizes.size()) {
				return sections_sizes[_section];
			}
			else return NULL;
		}

		DLL_API u64 sizeof_allocated() noexcept {
			return allocated_size;
		}

		DLL_API void* allocate(size_t count, core::memory::tag tag) noexcept {
			CORE_TRACE_CURRENT_FUNCTION();

			CRASH_IF(count < 1, "allocate zero count allocation not allowed !");

			void* pointer = new byte[count];

			VCRASH_IF(pointer == nullptr, "failed to allocate memory with size = {}byte !" , count);

			// update total count
			allocated_size += count;

			// update section count
			u8 _section = (u8)tag;
			if (_section < sections_sizes.size()) {
				sections_sizes[_section] += count;
			}
			else sections_sizes[0] += count;

			// update allocate list
			allocations_list.insert({
				(ptr)pointer , alloc_info{count , _section}
			});

			CORE_TRACE(
				"'{} byte' allocated for section '{}'",
				count, core::memory::tag_to_string(tag)
			);
			
			return pointer;
		}

		DLL_API void deallocate(void* pointer) noexcept {
			CORE_TRACE_CURRENT_FUNCTION();

			CRASH_IF(pointer == nullptr, "attempt to deallocate null-pointer !");

			// check alloc_list
			auto allocation = allocations_list.find((ptr64)pointer);

			VCRASH_IF(
				allocation == allocations_list.end(), 
				"attempt to deallocate invalid-pointer {} !" , 
				core::pointer_to_hex_string(pointer)
			);

			// update total count
			allocated_size = (allocated_size - allocation->second.count) < 0 ? 0 : allocated_size - allocation->second.count;

			// update section count
			u8 _section = allocation->second.memory_section;
			if (_section < sections_sizes.size()) {
				sections_sizes[_section] = (sections_sizes[_section] - allocation->second.count) < 0 ? 0 : (sections_sizes[_section] - allocation->second.count);
			}
			else {
				// todo: improve this by solving this "bug" situation !
				// out of range _section mean there's a bug in allocate
				CORE_CRASH(true, "out-of-range _section_ mean there's a bug !");
			}

			// update allocate list
			allocations_list.erase(allocation->first);

			delete[] pointer;

			CORE_TRACE(
				"0x{} deallocated from '{}' section", 
				pointer, core::memory::tag_to_string((core::memory::tag)_section)
			);
		}

		// todo: move this to core::to_string()
		DLL_API std::string tag_to_string(core::memory::tag _tag_) {
		
			#define CASE_TAG_TO_STR(TAG) case (core::memory::tag:: TAG) : return #TAG; break;
			switch (_tag_) {
				CASE_TAG_TO_STR(general);
				CASE_TAG_TO_STR(event_system);
				CASE_TAG_TO_STR(work_system);
				CASE_TAG_TO_STR(assets);
				CASE_TAG_TO_STR(gui);
				CASE_TAG_TO_STR(graphics);
				CASE_TAG_TO_STR(physics);
				CASE_TAG_TO_STR(collision);
				CASE_TAG_TO_STR(animations);
				CASE_TAG_TO_STR(ai);
				CASE_TAG_TO_STR(audio);
				CASE_TAG_TO_STR(stdcpp);
				CASE_TAG_TO_STR(core);
				CASE_TAG_TO_STR(files);
				default : return "unkown";
			}

		}

	} // namespace memory end

} // namespace core end

/*
	core::memory_allocator defintion
*/

core::memory_allocator::memory_allocator(std::string const& name, core::memory::tag memory_tag) noexcept 
	: _name_(name), _tag_(memory_tag) 
{
	CORE_DEBUG_D(" constructed()");
}

void* core::memory_allocator::allocate(u32 size) noexcept {
	VCRASH_IF(
		size >= core::memory_allocator::max_size_allowed , 
		"size={} size to big 'unsupported' !" , size
	);
	
	return core::memory::allocate(size, this->_tag_);
}

void core::memory_allocator::deallocate(void* pointer) noexcept {
	core::memory::deallocate(pointer);
}

std::string core::memory_allocator::get_name() noexcept {
	return this->_name_;
}

core::memory::tag core::memory_allocator::get_tag() noexcept {
	return this->_tag_;
}

#endif