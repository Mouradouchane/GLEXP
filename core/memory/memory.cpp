#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_CPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_CPP

#include "libs/mimalloc/mimalloc.h"
#include "libs/mimalloc/mimalloc-stats.h"

#include "core/logger/logger.hpp"
#include "memory.hpp"

#ifdef DEBUG
static auto _core_global_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
static auto _core_global_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_global_alloc_logger_

#ifdef DEBUG
// array for keep tracking total size of each section
#define SECTIONS_SIZE 64
static std::array<u64, SECTIONS_SIZE> sections_sizes = { 0u };
#endif

static u64 total_size = 0;


DLL_API const string& core::to_string(core::memory_tag tag) NOEXP {
	return string("todo: implement to_string for core::memory_tag !");
}

DLL_API const string& core::to_string(core::memory_tag section, u8 tag) NOEXP {
	return string("todo: implement to_string for sections with u8 tags !");
}

DLL_API void* core::memory::allocate(g_memory_request const& request) NOEXP {

#ifdef DEBUG
	// allocate memory
	void* ptr = mi_malloc(request.size + 1);
	// mark it 
	if (ptr) {
		u8* tptr = ((u8*)ptr + request.size);
		*tptr = (u8)request.tag;

		// update list
		sections_sizes[(u8)request.tag] += request.size;
		total_size += request.size;
		return ptr;
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request.size, request.tag);
		return nullptr;
	}

#else
	void* ptr = mi_malloc(request.size);
	total_size += request.size;

	return ptr;
#endif

}

DLL_API tow_pointers core::memory::allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP {

#ifdef DEBUG

	// allocate memory
	void* ptr1 = mi_malloc(request_1.size + 1);
	void* ptr2 = mi_malloc(request_2.size + 1);

	// try mark the memory with tag
	if ((ptr1 && ptr2) && request_1.tag < SECTIONS_SIZE && request_2.tag < SECTIONS_SIZE) {
		u8* tptr1 = ((u8*)ptr1 + request_1.size);
		*tptr1 = (u8)request_1.tag;

		u8* tptr2 = ((u8*)ptr2 + request_2.size);
		*tptr2 = (u8)request_2.tag;

		// update list
		sections_sizes[(u8)request_1.tag] += request_1.size;
		sections_sizes[(u8)request_2.tag] += request_2.size;

		total_size += request_1.size + request_2.size;

		return tow_pointers{ ptr1 , ptr2 };
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_1.size, request_1.tag);
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_2.size, request_2.tag);

		return tow_pointers{ nullptr , nullptr };
	}

#else
	tow_pointers tptr{
		mi_malloc(request_1.size) ,
		mi_malloc(request_2.size)
	};

	total_size += request_1.size + request_2.size;

	return tptr;
#endif

}

DLL_API void core::memory::deallocate(void* pointer) NOEXP {

	u32 size = mi_usable_size(pointer);
	u8  tag = *((u8*)pointer + (size - 1));

	mi_free(pointer);

	total_size -= size;
	sections_sizes[tag] -= size;

}


DLL_API u64 core::memory::total_memory_usage() NOEXP {
	return total_size;
}

DLL_API u64 core::memory::current_memory_usage(u8 section_tag) NOEXP {
	return (section_tag < SECTIONS_SIZE) ? sections_sizes[ section_tag ] : 0;
}

DLL_API u64 peak_memory_usage() NOEXP {

	mi_stats_t_decl(global_memory_stats);

	if (mi_stats_get(&global_memory_stats)) {
		return global_memory_stats.malloc_requested.peak;
	}
	else return 0;

}


INLINE void core::memory_allocation::clear() NOEXP {

#ifdef DEBUG
	this->ptr = nullptr;
	this->size = 0;
	this->tag = (u8)core::memory_tag::unkown;
#else 
	this->ptr = nullptr;
	this->size = 0;
#endif

}

#endif