#pragma once 

#ifndef CORE_DYNAMIC_ALLOCATOR_HPP
#define CORE_DYNAMIC_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/locks/atomic_types.hpp"
#include "core/memory/dynamic/block/block.hpp"
#include "core/strings/string.hpp"

typedef std::atomic<core::memory_block*> atomic_block_ptr;

namespace core {

	struct dynamic_allocator_configs {
		DEBUG_ONLY string name;
		// for what sub-system this allocator is gonna be used
		DEBUG_ONLY core::allocator_tag tag;
		/*
			memory budget is your blocks_count* blocks_size
		*/
		u64 blocks_size;  // size of each block
		u8  blocks_count; // 255 blocks max

		/*
			how many allocations each memory block can maintain
			note: the lowest the number , the better the preformance
		*/ 
		u16 max_allocations_per_block; 

		// is this allocator gonna be used by multiple threads/sub-system or not
		bool is_multi_thread;
	};

	/*
		- core::dynamic_allocator handle dynamic memory allocation with different size's like new/malloc .
		- note: this allocator run on limited memory budget , if he run's out of memory in a "crash" or "nullptr" you get .
		- note: this allocator is designed for multi-threaded cabable of "parallel-allocations" !
		        so the more blocks he have ready to be used , the more he can handle at the same-time .

		- how it's work ?
			- the allocator hold a list of blocks , thier size and count already known .
			- each block can be free or busy handling some allocation request .
			- the allocator loops through all the blocks until he find a "none-busy" block ready to serve .
			- this way the allocator can handle multiple allocations in parallel  .
	*/
	DLL_API_CLASS dynamic_allocator {
	private:
		core::memory_block* _blocks_  = nullptr; // blocks array
		u8                  _blocks_count_  = 0; 
		u64                 _blocks_size_   = core::dynamic_allocator::min_size_allowed;
		u64                 _size_          = 0; // size of all blocks
		u16                 _blocks_max_allocations_  = core::dynamic_allocator::max_allocations_per_block;

		
		DEBUG_ONLY atomic_u32 _peak_ = 0;          // peak memory usage
		DEBUG_ONLY atomic_u32 _min_  = 0xFFFFFFFF; // min  memory usage
		
		bool is_mt = false; // flag for MT or ST usage

	#ifdef DEBUG
		string              _name_; // allocator name for debug 
		core::allocator_tag _tag_; // allocator main usage tag for debug

		// used for "debugging purposes" to keep track of memory usage
		atomic_u32 _sections_[MAX_MEMORY_TAGS] = { 0u };
	#endif

	public:
		// public variables for usage 
		// min/max allowed size for dynamic_allocator "blocks"
		static const u64 min_size_allowed =  64 KB;
		static const u64 max_size_allowed = 512 MB;
		static const u16 max_allocations_per_block = 512;
		
		// constructor
		dynamic_allocator( core::dynamic_allocator_configs const& parameters ) NOEXP;

		// destructor
		~dynamic_allocator() NOEXP;

		/*
			dynamic_allocator public functions
		*/ 

		void* allocate(u32 size, u8 tag = 0) NOEXP;
		void* allocate(u32 size, u16 alignement = 0, u8 tag = 0) NOEXP;
		void* allocate(core::memory_request const& request) NOEXP;

		// allocate 2 memory chunks next to each other in one call
		two_pointers allocate_2(core::memory_request const& request_1 , core::memory_request const& request_2) NOEXP;

		void deallocate(void* pointer) NOEXP;

		u64 size() NOEXP; 
		u64 blocks_size() NOEXP; // size of each block
		u32 blocks_count() NOEXP; // return's how many memory block in this allocator

		u64    peak_memory_usage() NOEXP; // max memory usage reached by this allocation
		u64 current_memory_usage() NOEXP; // for all sections
		u64 current_memory_usage(core::memory_tag section_tag) NOEXP; // for specific section
		
	#ifdef DEBUG
		DEBUG_ONLY string const& name() NOEXP;
		DEBUG_ONLY core::allocator_tag tag() NOEXP;
	#else
		DEBUG_ONLY INLINE string const& name() NOEXP;
		DEBUG_ONLY INLINE core::allocator_tag tag() NOEXP;
	#endif
	
	private:
		// not allowed contructor's
		dynamic_allocator() = delete;
		dynamic_allocator(dynamic_allocator &&     other) = delete;
		dynamic_allocator(dynamic_allocator const& other) = delete;

		// not allowed operator's
		dynamic_allocator& operator = (const dynamic_allocator        other) = delete;
		dynamic_allocator& operator = (const dynamic_allocator &&     other) = delete;
		dynamic_allocator& operator = (const dynamic_allocator const& other) = delete;

	};
	// class dynamic_allocator end


} // namesapce core end


#endif