#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_CPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"
#include "memory.hpp"

static std::shared_ptr<spdlog::logger> _core_global_alloc_logger_ = nullptr;
#define _LOGGER_ _core_global_alloc_logger_

#define GLOBAL_ALLOCATOR_NAME "global allocator"
#define GLOBAL_ALLOCATOR_HIGH_MEMORY_USAGE_WARN() \
		CORE_WARN(0, HIGH_MEMORY_USAGE_DETECTED , GLOBAL_ALLOCATOR_NAME , peak_size);

#ifdef DEBUG // to keep track with each sub-system memory usage
	static u64 sections_sizes[MAX_MEMORY_TAGS] = { 0u };
#endif

	static u64 total_size = 0; // size of all sections
	static u64 peak_size  = 0; // max memory we reach

static bool is_init = false;
DLL_API void core::memory::init() NOEXP {
	if (!is_init) {
	#ifdef DEBUG
		_core_global_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
	#else 
		_core_global_alloc_logger_ = nullptr;
	#endif

	#ifdef DEBUG
		for (u8 i = 0; i < 64; i += 1) {
			CORE_DEBUG(0,"memory-tag: {} {}", i , core::to_string((memory_tag)i) );
		}
		for (u8 i = 0; i < 16; i += 1) {
			CORE_TRACE("allocator-tag: {} {}", i, core::to_string((allocator_tag)i));
		}
	#endif

		is_init = true;
	}
}

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
			GLOBAL_ALLOCATOR_HIGH_MEMORY_USAGE_WARN();
		}
	#endif

		return g_memory_handle(allocator_response::success, request.size, request.tag, ptr);
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request.size, core::to_string(request.tag));
		return g_memory_handle();
	}

}

DLL_API g_memory_handle_2 core::memory::allocate_tow(g_memory_request const& request_1, g_memory_request const& request_2) NOEXP {
	DEBUG_BREAK;

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
			GLOBAL_ALLOCATOR_HIGH_MEMORY_USAGE_WARN();
		}
	#endif

		return g_memory_handle_2{
			g_memory_handle(allocator_response::success, request_1.size, request_1.tag, ptr1),
			g_memory_handle(allocator_response::success, request_2.size, request_2.tag, ptr2)
		};
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_1.size, core::to_string(request_1.tag));
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_2.size, core::to_string(request_2.tag));

		return g_memory_handle_2{
			g_memory_handle(),
			g_memory_handle()
		};
	}

}

DLL_API void core::memory::deallocate(g_memory_handle const& handle) NOEXP {
	// DEBUG_BREAK;

	if (handle._ptr_) {
		u8 tag = *((u8*)handle._ptr_ + handle._size_);

		// release memory
		delete[] handle._ptr_;

		// update internal variables
		total_size -= handle._size_;
	#ifdef DEBUG 
		sections_sizes[tag] -= handle._size_;
	#endif

		CORE_DEBUG(0, "global-allocator: {} {}bytes used for {} deallocated ." , 
			core::pointer_to_hex_string(handle._ptr_) , handle._size_, core::to_string((memory_tag)tag)
		);
		return;
	}

	CORE_FATAL(CORE_LOG_CONFIG_ALL, INVALID_MEMORY_HANLDE, "global-allocator at core::memory::deallocate()");
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

#define MEMORY_TAG_TO_STRING_CASE(TAG) case memory_tag::TAG: return string( #TAG ); break;

DLL_API string core::to_string(memory_tag tag) NOEXP {
	
	switch (tag) {
		MEMORY_TAG_TO_STRING_CASE(unkown		);
		MEMORY_TAG_TO_STRING_CASE(general		);
		MEMORY_TAG_TO_STRING_CASE(registry		);
		MEMORY_TAG_TO_STRING_CASE(event			);
		MEMORY_TAG_TO_STRING_CASE(thread_worker	);
		MEMORY_TAG_TO_STRING_CASE(entity		);
		MEMORY_TAG_TO_STRING_CASE(mesh			);
		MEMORY_TAG_TO_STRING_CASE(node			);
		MEMORY_TAG_TO_STRING_CASE(gui_element	);
		MEMORY_TAG_TO_STRING_CASE(gui_text		);
		MEMORY_TAG_TO_STRING_CASE(normal		);
		MEMORY_TAG_TO_STRING_CASE(texture		);
		MEMORY_TAG_TO_STRING_CASE(material		);
		MEMORY_TAG_TO_STRING_CASE(physics		);
		MEMORY_TAG_TO_STRING_CASE(collision		);
		MEMORY_TAG_TO_STRING_CASE(skeleton		);
		MEMORY_TAG_TO_STRING_CASE(ai			);
		MEMORY_TAG_TO_STRING_CASE(audio			);
		MEMORY_TAG_TO_STRING_CASE(stdcpp		);
		MEMORY_TAG_TO_STRING_CASE(file			);
		MEMORY_TAG_TO_STRING_CASE(config		);
		MEMORY_TAG_TO_STRING_CASE(timer			);
		MEMORY_TAG_TO_STRING_CASE(debugger		);

	#ifdef DEBUG
		MEMORY_TAG_TO_STRING_CASE(dev           );
	#endif
	};

	return string("");
}

#define ALLOCATOR_TAG_TO_STRING_CASE(TAG) case allocator_tag:: TAG: return string( #TAG ); break;

DLL_API string core::to_string(allocator_tag tag) NOEXP {
	
	switch (tag) {
		ALLOCATOR_TAG_TO_STRING_CASE(unkown				 );
		ALLOCATOR_TAG_TO_STRING_CASE(memory_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE(assets_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE(entity_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE(events_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE(physics_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE(graphics_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE(collision_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE(animation_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE(work_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE(gui_system          );

	#ifdef DEBUG
		ALLOCATOR_TAG_TO_STRING_CASE(debug_system        );
	#endif
	};

	return string("");
}

#endif