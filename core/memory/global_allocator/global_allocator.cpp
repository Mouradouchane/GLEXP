#pragma once 

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_CPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"
#include "global_allocator.hpp"

#ifdef DEBUG
	static auto _core_global_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_global_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_global_alloc_logger_

// array for keep tracking total size of each section
static std::array<u64, 64> sections_sizes = { 0u };

static 
DLL_API std::string core::allocator::tag_to_string(core::memory_tag tag) NOEXP {

}

DLL_API void* core::allocator::allocate(u32 size, core::memory_tag memory_tag) NOEXP {

}

DLL_API tow_pointers core::allocator::allocate_tow(u32 size_1, u32 size_2, core::memory_tag tag) NOEXP {

}

DLL_API void core::allocator::deallocate(void* pointer) NOEXP {

}

DLL_API u64 core::allocator::total_used_memory() NOEXP {

}

DLL_API u64 core::allocator::used_memory(core::memory_tag memory_tag) NOEXP {

}


#endif
