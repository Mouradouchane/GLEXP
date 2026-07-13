#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_CPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"
#include "memory.hpp"

#ifdef DEBUG
	static auto _core_global_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_global_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_global_alloc_logger_

#ifdef DEBUG // to keep track with each sub-system memory usage
	static u64 sections_sizes[MAX_MEMORY_TAGS] = { 0u };
#endif
	static u64 total_size = 0; // size of all sections
	static u64 peak_size  = 0; // max memory we reach

DLL_API g_memory_handle core::memory::allocate(g_memory_request const& request) NOEXP {

	// allocate memory
	void* ptr = new byte[request.size + 1];

	if (ptr) {

	#ifdef DEBUG
		// tag memory
		u8* tptr = ((u8*)ptr + request.size);
		*tptr = (u8)request.tag;

		// update list
		sections_sizes[(u8)request.tag] += request.size;
	#endif

		total_size += request.size;

	#ifdef DEBUG
		if (total_size > peak_size) {
			peak_size = total_size;
			CORE_WARN(0, G_HIGH_MEMORY_USAGE_DETECTED);
		}
	#endif

		return g_memory_handle(allocator_response::success, request.size, request.tag, ptr);
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request.size, request.tag);
		return g_memory_handle();
	}

}

DLL_API g_memory_handle_2 core::memory::allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP {

	// allocate memory
	void* ptr1 = new byte[request_1.size + 1];
	void* ptr2 = new byte[request_2.size + 1];

	// try mark the memory with tag
	if ((ptr1 && ptr2) && (u8)request_1.tag < MAX_MEMORY_TAGS && (u8)request_2.tag < MAX_MEMORY_TAGS) {
	
	#ifdef DEBUG
		// tag memory
		u8* tptr1 = ((u8*)ptr1 + request_1.size);
		*tptr1 = (u8)request_1.tag;

		u8* tptr2 = ((u8*)ptr2 + request_2.size);
		*tptr2 = (u8)request_2.tag;

		// update list
		sections_sizes[(u8)request_1.tag] += request_1.size;
		sections_sizes[(u8)request_2.tag] += request_2.size;
	#endif

		total_size += request_1.size + request_2.size;

	#ifdef DEBUG
		if (total_size > peak_size) {
			peak_size = total_size;
			CORE_WARN(0, G_HIGH_MEMORY_USAGE_DETECTED);
		}
	#endif

		return g_memory_handle_2{
			g_memory_handle(allocator_response::success, request_1.size, request_1.tag, ptr1),
			g_memory_handle(allocator_response::success, request_2.size, request_2.tag, ptr2)
		};
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_1.size, request_1.tag);
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_2.size, request_2.tag);

		return g_memory_handle_2{
			g_memory_handle(),
			g_memory_handle()
		};
	}

}

DLL_API void core::memory::deallocate(g_memory_handle const& handle) NOEXP {

	if (handle._ptr_) {
		// free memory
		delete[] handle._ptr_;

		u32 size = handle._size_;
		u8  tag = *((u8*)handle._ptr_ + (size - 1));

		// update section
		total_size -= size;
	
	#ifdef DEBUG
		sections_sizes[tag] -= size;
	#endif

		return;
	}

	CORE_FATAL(CORE_LOG_CONFIG_ALL, G_INVALID_HANLDE, "core::memory::deallocate()");
}


DLL_API u64 core::memory::total_memory_usage() NOEXP {
	return total_size;
}

DLL_API u64 core::memory::current_memory_usage(allocator_tag section_tag) NOEXP {
#ifdef DEBUG
	return ((u8)section_tag < MAX_MEMORY_TAGS) ? sections_sizes[ (u8)section_tag ] : 0;
#else
	return 0;
#endif
}

// todo: calc peak memory usage
DLL_API u64 core::memory::peak_memory_usage() NOEXP {
#ifdef DEBUG
	return peak_size;
#else
	return 0;
#endif
}


/*
	class memory_handle
*/

memory_handle::memory_handle(allocator_response response, u8 b_index, u32 reg_index, void* pointer) NOEXP
	: _response_(response), _block_index_(b_index), _register_index_(reg_index), _ptr_(pointer) 
{
	CORE_DEBUG(0, "new memory handle created : response={} , pointer={} ." , 
		(u8)response , core::pointer_to_hex_string(pointer)
	);
};

memory_handle::~memory_handle() NOEXP {
	this->_ptr_ = nullptr;
}

allocator_response memory_handle::response() NOEXP {
	return this->_response_;
}

void* memory_handle::get_pointer() NOEXP {
	return this->_ptr_;
}


/*
	class g_memory_handle
*/
g_memory_handle::g_memory_handle(allocator_response response, u64 size, allocator_tag tag, void* pointer) NOEXP
	: _response_(response), _size_(size), _tag_(tag), _ptr_(pointer) 
{
	CORE_DEBUG(
		0, "new global memory handle created . response={} , size={} , tag={} , pointer={}",
		(u8)response, size, (u8)tag, core::pointer_to_hex_string(pointer)
	);
}

g_memory_handle::~g_memory_handle() NOEXP {
	this->_ptr_ = nullptr;
}

allocator_response g_memory_handle::response() NOEXP {
	return this->_response_;
}

void* g_memory_handle::get_pointer() NOEXP {
	return this->_ptr_;
}


/*
	to string functions
*/

DLL_API string core::to_string(memory_tag tag) NOEXP {
	CORE_FATAL_F(CORE_TODO_IMPLEMENT);
	return string("");
}

DLL_API string core::to_string(allocator_tag tag ) NOEXP {
	CORE_FATAL_F(CORE_TODO_IMPLEMENT);
	return string("");
}

#endif