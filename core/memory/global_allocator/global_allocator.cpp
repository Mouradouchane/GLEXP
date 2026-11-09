#pragma once

#ifndef CORE_GLOBAL_MEMORY_ALLOCATOR_CPP
#define CORE_GLOBAL_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"
#include "core/bitset.hpp"
#include "core/memory/global_allocator/io/global_allocator_io.hpp"
#include "global_allocator.hpp"

static std::shared_ptr<spdlog::logger> _core_global_alloc_logger_ = nullptr;
#define _LOGGER_ _core_global_alloc_logger_

#define GLOBAL_ALLOCATOR_NAME "global allocator"
#define GLOBAL_ALLOCATOR_HIGH_MEMORY_USAGE_WARN() \
		CORE_WARN(0, HIGH_MEMORY_USAGE_DETECTED, GLOBAL_ALLOCATOR_NAME, peak_size);

#ifdef DEBUG // to keep track with each sub-system memory usage
	static u64 sections_sizes[MAX_MEMORY_TAGS] = { 0u };
#endif

static u64 total_size = 0; // size of all sections
static u64 peak_size  = 0; // max memory we reach

static bool is_init = false;

DLL_API void core::global_allocator::init() NOEXP {
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

DLL_API memory_handle_g core::global_allocator::allocate(memory_request_g const& request) NOEXP {

	// allocate memory
	void* ptr = new byte[request.size];

	if (ptr) {

		// update variables
	#ifdef DEBUG
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

		return memory_handle_g(allocator_response::success, request.size, request.tag, ptr);
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request.size, core::to_string(request.tag));
		return memory_handle_g();
	}

}

DLL_API same_pair<memory_handle_g> core::global_allocator::allocate_tow(memory_request_g const& request_1, memory_request_g const& request_2) NOEXP {

	// allocate memory
	void* ptr1 = new byte[request_1.size];
	void* ptr2 = new byte[request_2.size];

	// try mark the memory with tag
	if ((u8)request_1.tag < MAX_MEMORY_TAGS && (u8)request_2.tag < MAX_MEMORY_TAGS) {
	
	#ifdef DEBUG
		// update variables
		sections_sizes[(u8)request_1.tag] += request_1.size;
		sections_sizes[(u8)request_2.tag] += request_2.size;
	#endif

		total_size += request_1.size + request_2.size;

	#ifdef DEBUG
		if (total_size > peak_size) {
			peak_size = total_size;
			GLOBAL_ALLOCATOR_HIGH_MEMORY_USAGE_WARN();
		}

		CORE_DEBUG(0, "global-allocator: new pair memory allocated {} for {} and {}.",
			core::bytes_to_string(request_1.size + request_2.size), 
			core::to_string(request_1.tag), core::to_string(request_2.tag)
		);
	#endif

		return same_pair<memory_handle_g> {
			memory_handle_g(allocator_response::success, request_1.size, request_1.tag, ptr1),
			memory_handle_g(allocator_response::success, request_2.size, request_2.tag, ptr2)
		};
	}
	else {
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_1.size, core::to_string(request_1.tag));
		CORE_FATAL_F(GLOBAL_ALLOCATOR_FAILED, request_2.size, core::to_string(request_2.tag));

		return same_pair<memory_handle_g>{
			memory_handle_g(),
			memory_handle_g()
		};
	}

}

DLL_API void core::global_allocator::deallocate(memory_handle_g& handle) NOEXP {

	if (handle._ptr_) {

		// deallocate memory
		delete[] handle._ptr_;

		// update allocator variables
		total_size -= handle._size_;

	#ifdef DEBUG 
		if( (u8)handle._tag_ < MAX_MEMORY_TAGS ){
			sections_sizes[(u8)handle._tag_] -= handle._size_;

			CORE_DEBUG(0, "global-allocator: memory {} is deallocated , used for {} size {}." , 
				core::pointer_to_hex_string(handle._ptr_), core::to_string(handle._tag_),
				core::bytes_to_string(handle._size_)
			);
		}
		else {
			CORE_WARN(0,"global-allocator: memory {} have invalid tag {} " , 
				core::pointer_to_hex_string(handle._ptr_), core::to_string(handle._tag_)
			);
		}
	#endif

		// make handle invalid
		handle._ptr_ = nullptr;
		
		return;
	}

	CORE_FATAL(CORE_LOG_CONFIG_ALL, INVALID_MEMORY_HANLDE, "global-allocator");
}


DLL_API u64 core::global_allocator::current_memory_usage() NOEXP {
	return total_size;
}

DLL_API u64 core::global_allocator::current_memory_usage(subsystem_memory_tag section_tag) NOEXP {
#ifdef DEBUG
	u8 index = (u8)section_tag;
	return ( index < MAX_MEMORY_TAGS) ? sections_sizes[ index ] : 0;
#else
	return 0;
#endif
}

DLL_API u64 core::global_allocator::peak_memory_usage() NOEXP {
#ifdef DEBUG
	return peak_size;
#else
	return 0;
#endif
}

/*
	to string functions
*/

#define MEMORY_TAG_TO_STRING_CASE(TAG) case memory_tag::TAG: return string( #TAG ); break;

DLL_API string core::to_string(memory_tag _tag_) NOEXP {
	
	switch (_tag_) {
		MEMORY_TAG_TO_STRING_CASE( unkown		 );
		MEMORY_TAG_TO_STRING_CASE( general		 );
		MEMORY_TAG_TO_STRING_CASE( registry		 );
		MEMORY_TAG_TO_STRING_CASE( event		 );
		MEMORY_TAG_TO_STRING_CASE( thread_worker );
		MEMORY_TAG_TO_STRING_CASE( entity		 );
		MEMORY_TAG_TO_STRING_CASE( mesh			 );
		MEMORY_TAG_TO_STRING_CASE( node			 );
		MEMORY_TAG_TO_STRING_CASE( gui_element	 );
		MEMORY_TAG_TO_STRING_CASE( gui_text		 );
		MEMORY_TAG_TO_STRING_CASE( normal		 );
		MEMORY_TAG_TO_STRING_CASE( texture		 );
		MEMORY_TAG_TO_STRING_CASE( material		 );
		MEMORY_TAG_TO_STRING_CASE( physics		 );
		MEMORY_TAG_TO_STRING_CASE( collision	 );
		MEMORY_TAG_TO_STRING_CASE( skeleton		 );
		MEMORY_TAG_TO_STRING_CASE( ai			 );
		MEMORY_TAG_TO_STRING_CASE( audio		 );
		MEMORY_TAG_TO_STRING_CASE( stdcpp		 );
		MEMORY_TAG_TO_STRING_CASE( file			 );
		MEMORY_TAG_TO_STRING_CASE( config		 );
		MEMORY_TAG_TO_STRING_CASE( timer		 );
		MEMORY_TAG_TO_STRING_CASE( debugger		 );

	#ifdef DEBUG
		MEMORY_TAG_TO_STRING_CASE( dev           );
	#endif
	};

	return string("");
}

#define ALLOCATOR_TAG_TO_STRING_CASE(TAG) case subsystem_memory_tag:: TAG: return string( #TAG ); break;

DLL_API string core::to_string(subsystem_memory_tag _tag_) NOEXP {
	
	switch (_tag_) {
		ALLOCATOR_TAG_TO_STRING_CASE( unkown			 );
		ALLOCATOR_TAG_TO_STRING_CASE( memory_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE( assets_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE( entity_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE( events_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE( physics_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE( graphics_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE( collision_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE( animation_system	 );
		ALLOCATOR_TAG_TO_STRING_CASE( work_system		 );
		ALLOCATOR_TAG_TO_STRING_CASE( gui_system         );

	#ifdef DEBUG
		ALLOCATOR_TAG_TO_STRING_CASE( debug_system       );
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