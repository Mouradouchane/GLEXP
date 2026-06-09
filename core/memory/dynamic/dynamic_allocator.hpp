#pragma once 

#ifndef CORE_DYNAMIC_ALLOCATOR_HPP
#define CORE_DYNAMIC_ALLOCATOR_HPP

#include <array>

#include "core/strings/string.hpp"

#include "core/memory/memory.hpp"
#include "core/memory/dynamic/block/block.hpp"

#define MAX_SECTIONS 255

namespace core {

	DLL_API_CLASS dynamic_allocator {
	private:
		core::memory_block* blocks = nullptr;
		u32                 blocks_count = 0;

		bool _multi_threaded_ = false;

	#ifdef DEBUG // debug-only variables
		string _name_;
		core::memory_tag _tag_;

		// used to keep track of memory for "debugging purposes"
		std::array<u32, MAX_SECTIONS> sections_sizes = { 0u };
	#endif

	public:
		// min/max allowed size for dynamic_allocator
		static const u64 min_size_allowed = 128  KB;
		static const u64 max_size_allowed = 1024 MB;
		
		// constructor
		dynamic_allocator( core::allocator_parameters const& parameters ) NOEXP;

		// destructor
		~dynamic_allocator() NOEXP;

		// dynamic_allocator public functions
		void* allocate(u32 size , u8 tag = 0) NOEXP;
		void* allocate(core::memory_request const& request) NOEXP;

		// allocate tow memory chunks next to each other in one call
		tow_pointers allocate_tow(core::memory_request const& request_1 , core::memory_request const& request_2) NOEXP;

		void deallocate(void* pointer) NOEXP;

		u64 total_memory() NOEXP;

		// for all sections
		u64 current_memory_usage() NOEXP; 
		// for one section only
		u64 current_memory_usage(u8 section_tag) NOEXP;

		u64 peak_memory_usage() NOEXP;

	#ifdef DEBUG
		const string&    name() NOEXP;
		core::memory_tag tag()  NOEXP;
	#else 
		INLINE const string&    name() NOEXP;
		INLINE core::memory_tag tag()  NOEXP;
	#endif

	private: 

		// not allowed contructor's
		dynamic_allocator() = delete;
		dynamic_allocator(dynamic_allocator &      other) = delete;
		dynamic_allocator(dynamic_allocator &&     other) = delete;
		dynamic_allocator(dynamic_allocator const& other) = delete;

		// not allowed operator's
		dynamic_allocator& operator = (const dynamic_allocator        other) = delete;
		dynamic_allocator& operator = (const dynamic_allocator &      other) = delete;
		dynamic_allocator& operator = (const dynamic_allocator &&     other) = delete;
		dynamic_allocator& operator = (const dynamic_allocator const& other) = delete;

	};
	// class dynamic_allocator end


} // namesapce core end


#endif