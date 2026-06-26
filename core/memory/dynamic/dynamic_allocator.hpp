#pragma once 

#ifndef CORE_DYNAMIC_ALLOCATOR_HPP
#define CORE_DYNAMIC_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/locks/atomic_types.hpp"
#include "core/memory/dynamic/block/block.hpp"
#include "core/strings/string.hpp"

namespace core {

	struct dynamic_allocator_configs {
		DEBUG_ONLY string name;
		DEBUG_ONLY core::allocator_tag tag; // for what sub-system this allocator is gonna be used
		
		u64 memory_budget; // max memory this allocator should reach
		u64 blocks_size; // size of each block
		
		/*
			how many allocations each memory block can maintain
			note: the lowest the number , the better the preformance
		*/ 
		u16 max_allocations_per_block; 

		bool is_multi_thread; // is this allocator gonna be used by multiple threads/sub-system or not
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
		bool               _blocks_status_[MAX_MEMORY_BLOCKS] = { false }; // for the status of each block
		core::memory_block _blocks_[MAX_MEMORY_BLOCKS]; // blocks array
		u8 const           _capacity_ = MAX_MEMORY_BLOCKS; // max allowed blocks
		u8                 _blocks_count_  = 0; 
		u64                _blocks_size_ = core::dynamic_allocator::min_size_allowed;
		u16                _blocks_max_allocations_; // max allocations allowed per block

		u64          _memory_budget_ = 0; // max allowed size
		
		atomic_u32            _size_ = 0; // total memory "currentlly"
		DEBUG_ONLY atomic_u32 _peak_ = 0;          // peak memory usage
		DEBUG_ONLY atomic_u32 _min_  = 0xFFFFFFFF; // min  memory usage
		
		bool _is_mt_ = false; // is this allocator for multi-threaded usage ?

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
		static const u16 max_allocations_per_block = 1024;
		
		// constructor
		dynamic_allocator( core::dynamic_allocator_configs const& parameters ) NOEXP;

		// destructor
		~dynamic_allocator() NOEXP;

		/*
			dynamic_allocator public functions
		*/

		INLINE core::memory_handle allocate(u32 size, u8 tag = 0) NOEXP;
		INLINE core::memory_handle allocate(u32 size, u16 alignement = 0, u8 tag = 0) NOEXP;
		       core::memory_handle allocate(core::memory_request const& request) NOEXP;

		// allocate 2 memory chunks next to each other in one call
		core::memory_handle_2 allocate_tow(core::memory_request const& request_1 , core::memory_request const& request_2) NOEXP;

		void deallocate(core::memory_handle handle) NOEXP;

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
	
	private: // helper functions
		INLINE void  remove_block(u8  block_index) NOEXP;
		INLINE u8   add_new_block(u32 block_size)  NOEXP;

		DEBUG_ONLY INLINE void add_size_to_section(u32 size, u8 section_tag);
		DEBUG_ONLY INLINE void remove_size_from_section(core::memory_handle handle);

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