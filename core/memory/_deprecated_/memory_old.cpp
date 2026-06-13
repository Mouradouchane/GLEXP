#pragma once 

#ifndef CORE_MEMORY_ALLOCATOR_CPP
#define CORE_MEMORY_ALLOCATOR_CPP

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

#include <algorithm>

#include "core/assert.hpp"
#include "core/status/status.hpp"
#include "core/logger/logger.hpp"

#include "memory.hpp"

#include "core/references/references.hpp"

#ifdef DEBUG
	static auto _core_mem_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_mem_alloc_logger_ = nullptr;
#endif

#define _LOGGER_ _core_mem_alloc_logger_ 

typedef struct alloc_info {
	size_t count;
	u8     memory_section;
};

// here is how much memory is allocated by each section
// note: look at core::memory::section !
static std::array<u64, 32> sections_sizes = { 0u };

namespace core {

	/*
		globabl memory allocator
	*/
	namespace memory {

		// global memory-allocator variables
		static u64 allocated_size = NULL;
		static std::map < ptr, alloc_info > allocations_list = {};
		// =========================================

		DLL_API u64 sizeof_section(core::memory::tag tag) NOEXP {
			u8 _section = (u8)tag;

			if (_section < sections_sizes.size()) {
				return sections_sizes[_section];
			}
			else return NULL;
		}

		DLL_API u64 sizeof_allocated() NOEXP {
			return allocated_size;
		}

		DLL_API void* allocate(size_t count, core::memory_tag tag) NOEXP {
			CORE_STACK_TRACE();

			CORE_FATAL_IF(count < 1, CORE_LOG_CONFIG_D , "{}" , "allocate zero count allocation not allowed !");

			void* pointer = new byte[count];

			CORE_FATAL_IF(pointer == nullptr, CORE_LOG_CONFIG_D, "{}", "failed to allocate memory with size = {}byte !" , count);

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

		DLL_API void deallocate(void* pointer) NOEXP {
			CORE_STACK_TRACE();

			CORE_FATAL_IF(pointer == nullptr, CORE_LOG_CONFIG_ALL , "{}", "attempt to deallocate null-pointer !");

			// check alloc_list
			auto allocation = allocations_list.find((ptr64)pointer);

			CORE_FATAL_IF(
				allocation == allocations_list.end(), 
				CORE_LOG_CONFIG_ALL, "attempt to deallocate invalid-pointer {} !" , 
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

core::memory_allocator::memory_allocator(
	std::string const& name, core::memory::config const& allocator_configs
) NOEXP
	: _name_(name), _tag_(allocator_configs.allocator_tag)
{

	CORE_FATAL_IF(
		(allocator_configs.allocator_size < min_size_allowed) || (allocator_configs.allocator_size > max_size_allowed),
		CORE_LOG_CONFIG_ALL, NOT_ALLOWED_ALLOCATOR_SIZE , allocator_configs.allocator_size , min_size_allowed , max_size_allowed
	);

	CORE_FATAL_IF(
		allocator_configs.max_allocation < 1, CORE_LOG_CONFIG_ALL, NOT_ALLOWED_ALLOCATION_LIST , 
		allocator_configs.max_allocation , 1, allocator_configs.allocator_size
	);

	// allocate memory + update variables
	this->max_allowed_allocations = allocator_configs.max_allocation;
	this->_size_ = allocator_configs.allocator_size;

	this->start  = (byte*)core::memory::allocate(this->_size_, allocator_configs.allocator_tag);
	this->end    = (byte*)(this->start + this->_size_);
	this->seek   = this->start;

	// init alloc lists
	this->alloc_list_size = (this->max_allowed_allocations + u32(this->max_allowed_allocations / 2u));

	this->alloc_list = (registry_pair*)core::memory::allocate(sizeof(registry_pair) * this->alloc_list_size);
	this->free_list  = (registry_pair*)core::memory::allocate(sizeof(registry_pair) * this->max_allowed_allocations);

	init_registry_list(this->alloc_list, this->alloc_list_size);
	init_registry_list(this->free_list, this->max_allowed_allocations);

	CORE_DEBUG(0,"new memory_allocator created name={} , category={} , size={}kb .", name, (u16)this->_tag_ , this->_size_);
}

/*
	destructor
*/
core::memory_allocator::~memory_allocator() NOEXP {

	core::memory::deallocate(this->start);

	/*
	
		todo[import]: calc how many allocation still in alloc_list, all of them is "memory-leaks"

	*/ 


	// =================================================

	core::memory::deallocate(this->alloc_list);
	core::memory::deallocate(this->free_list);

	this->start = nullptr;
	this->end   = nullptr;
	this->seek  = nullptr;

	this->alloc_list = nullptr;
	this->free_list  = nullptr;
}

template<typename type>
type* core::memory_allocator::allocate(u32 size) NOEXP {
	
}

void core::memory_allocator::deallocate(void* pointer) NOEXP {
	core::memory::deallocate(pointer);
}

std::string core::memory_allocator::get_name() NOEXP {
	return this->_name_;
}

core::memory::tag core::memory_allocator::get_tag() NOEXP {
	return this->_tag_;
}

#endif