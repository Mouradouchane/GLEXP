#pragma once 

#ifndef CORE_DYNAMIC_ALLOCATOR_HPP
#define CORE_DYNAMIC_ALLOCATOR_HPP

#include "core/strings/string.hpp"

#include "core/memory/registery/registery.hpp"
#include "core/memory/block/block.hpp"

namespace core {

	DLL_API_CLASS dynamic_allocator {
	private:
		core::memory::block *blocks;
		u32                  blocks_count;
			
	#ifdef DEBUG // debug-only variables
		STRING      name; 
		memory::tag tag;
	#endif

		// not allowed contructor's
		dynamic_allocator() = delete;
		dynamic_allocator(dynamic_allocator &  other)      = delete;
		dynamic_allocator(dynamic_allocator && other)      = delete;
		dynamic_allocator(dynamic_allocator const& other)  = delete;

		// not allowed operator's
		dynamic_allocator& operator = (const dynamic_allocator&  other)      = delete;
		dynamic_allocator& operator = (const dynamic_allocator&& other)      = delete;
		dynamic_allocator& operator = (const dynamic_allocator const& other) = delete;

	public:
		// min/max allowed size for dynamic_allocator
		static const u32 min_size_allowed =  64 KB;
		static const u32 max_size_allowed = 512 MB;

		// constructor
		dynamic_allocator( STRING const& allocator_name, core::memory::tag usage_tag, u32 memory_blocks_size ) NOEXP;

		// destructor 
		~dynamic_allocator() NOEXP;

		// dynamic_allocator public functions
		void* allocate(u32 size) NOEXP;
		void  deallocate(void* pointer) NOEXP;

		// tow memory-allocations in one call
		tow_pointers allocate_tow(u32 size_of_first, u32 size_of_second) NOEXP;

	#ifdef DEBUG
		STRING       get_name() NOEXP;
		memory::tag  get_tag()  NOEXP;
	#else 
		INLINE STRING       get_name() NOEXP;
		INLINE memory::tag  get_tag()  NOEXP;
	#endif

		u32 sizeof_all()       NOEXP;
		u32 sizeof_avalible()  NOEXP;
		u32 sizeof_allocated() NOEXP;

	};
	// class dynamic_allocator end


} // namesapce core end


#endif