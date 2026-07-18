#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_CPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"
#include "core/bitset.hpp"
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
			CORE_TRACE("allocator-tag: {} {}", i, core::to_string((subsystem_memory_tag)i));
		}

		// todo: move this to a "global allocator" unit test
		CORE_DEBUG(0, "BYTE");
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(1));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(2));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(32));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(1023));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(1024));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(2048));

		CORE_DEBUG(0,"KB");
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   1 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   2 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(1024 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(1025 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(2225 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(3500 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(9999 KB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(99999 KB));

		CORE_DEBUG(0, "MB");
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   1 MB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   2 MB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(  64 MB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string( 128 MB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(1024 MB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(2222 MB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(3579 MB));

		CORE_DEBUG(0, "GB");
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   1 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   2 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   3 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   7 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(   8 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(  64 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string( 512 GB));
		CORE_DEBUG(0, "memory unit to string: {}", core::bytes_to_string(2222 GB));

	#endif

		is_init = true;
	}

} // init end


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

		CORE_DEBUG(0, "global-allocator: new memory allocated {} for {}.",
			core::bytes_to_string(request.size), core::to_string(request.tag)
		);
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

		CORE_DEBUG(0, "global-allocator: new memory allocated {} for {}.",
			core::bytes_to_string(request_1.size), core::to_string(request_1.tag)
		);

		CORE_DEBUG(0, "global-allocator: new memory allocated {} for {}.",
			core::bytes_to_string(request_2.size), core::to_string(request_2.tag)
		);
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

	if (handle.ptr) {
		u8 tag = *((u8*)handle.ptr + handle._size_);

		// release memory
		delete[] handle.ptr;

		// update internal variables
		total_size -= handle._size_;
	#ifdef DEBUG 
		sections_sizes[tag] -= handle._size_;

		CORE_DEBUG(0, "global-allocator: memory {} is deallocated , used for {} size {}." , 
			core::pointer_to_hex_string(handle.ptr) , core::to_string((subsystem_memory_tag)tag),
			core::bytes_to_string(handle._size_)
		);
	#endif

		return;
	}

	CORE_FATAL(CORE_LOG_CONFIG_ALL, INVALID_MEMORY_HANLDE, "global-allocator");
}


DLL_API u64 core::memory::total_memory_usage() NOEXP {
	return total_size;
}

DLL_API u64 core::memory::current_memory_usage(subsystem_memory_tag section_tag) NOEXP {
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

memory_handle::memory_handle(allocator_response response_, u8 b_index, u32 reg_index, void* pointer) NOEXP
	: response(response_), _block_index_(b_index), _register_index_(reg_index), ptr(pointer) 
{
};

memory_handle::~memory_handle() NOEXP {

	this->ptr = nullptr;
}


/*
	class g_memory_handle
*/
g_memory_handle::g_memory_handle(
	allocator_response response_, u64 size, subsystem_memory_tag tag, void* pointer, 
	bool deallocate_at_destruction_time
) NOEXP
	: response(response_), _size_(size), _tag_(tag), ptr(pointer) , _deallocate_at_destuctor_(deallocate_at_destruction_time)
{

}

g_memory_handle::~g_memory_handle() NOEXP {
	if (this->_deallocate_at_destuctor_ && this->ptr) {
		core::memory::deallocate(*this);
	}

#ifdef DEBUG
	if (this->ptr) {
		CORE_WARN(core::logger::log_config::dump_stack_trace, 
			"'memory leak' posibility ! global memory handle destructed while still carrying a vaild pointer !"
		);
	}
#endif

	this->ptr = nullptr;
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

#define ALLOCATOR_TAG_TO_STRING_CASE(TAG) case subsystem_memory_tag:: TAG: return string( #TAG ); break;

DLL_API string core::to_string(subsystem_memory_tag tag) NOEXP {
	
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

typedef void (*ptrfn_byte_converter_)(string& str , u64& bytes_count) NOEXP;

void _to_byte_(string& str, u64& bytes_count) NOEXP { 
	str = std::to_string(bytes_count) + "BYTE";
}

void _to_kb_(string& str,   u64& bytes_count) NOEXP { 
	f64 kbs = BYTE_TO_KB(bytes_count);
	str = std::to_string(kbs) + "KB";
}

void _to_mb_(string& str,   u64& bytes_count) NOEXP {
	f64 mbs = BYTE_TO_MB(bytes_count);
	str = std::to_string(mbs) + "MB";
}

void _to_gb_(string& str,   u64& bytes_count) NOEXP { 
	f64 gbs = BYTE_TO_GB(bytes_count);
	str = std::to_string(gbs) + "GB";
}

DLL_API string core::bytes_to_string(u64 bytes_count) NOEXP {
	static const u64 _1kb_ = 1 KB;
	static const u64 _1mb_ = 1 MB;
	static const u64 _1gb_ = 1 GB;

	static const ptrfn_byte_converter_ jump_table[4] = {
		_to_byte_, _to_kb_, _to_mb_, _to_gb_
	};

	u8 r  = (bytes_count >= _1kb_);
	   r += (bytes_count >= _1mb_);
	   r += (bytes_count >= _1gb_);

	string str;

	jump_table[r](str, bytes_count);

	return str;
}

#endif