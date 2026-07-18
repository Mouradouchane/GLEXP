#pragma once 

#ifndef CORE_DYNAMIC_ALLOCATOR_HPP
#define CORE_DYNAMIC_ALLOCATOR_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/locks/atomic_lock/atomic_lock.hpp"
#include "core/locks/atomic_types.hpp"
#include "core/memory/memory.hpp"
#include "core/memory/dynamic/registery/registery.hpp"
#include "core/memory/dynamic/block/block.hpp"
#include "core/strings/string.hpp"

namespace core {

	struct dynamic_allocator_configs {
		DEBUG_ONLY string name;
		DEBUG_ONLY subsystem_memory_tag tag;
		
		u64 memory_budget; // max memory this allocator can reach and operates on

		/*
			how many allocations each memory block can maintain
			note: the lowest the number , the better the preformance
		*/ 
		u16 max_allocations_per_block;
		
		bool is_multi_thread; // is this allocator gonna be used by multiple threads/sub-system or not
		
		bool allocate_all_at_once; // allocate all memory at once
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
	#ifdef DEBUG
		string              _name_;
		subsystem_memory_tag _tag_;

		// used for "debugging purposes" to keep track of memory usage
		atomic_u32 _sections_[MAX_MEMORY_TAGS] = { 0u };
	#endif

		core::atomic_lock  _lock_;
		core::memory_block _blocks_[MAX_MEMORY_BLOCKS]; // blocks array
		bool               _blocks_status_[MAX_MEMORY_BLOCKS] = { false }; // for the status of each block
		u8 const           _capacity_    = MAX_MEMORY_BLOCKS; // max allowed blocks
		atomic_u8          _blocks_count_  = 0; 
		u16                _blocks_max_allocations_ = MAX_ALLOCATIONS_PRE_BLOCK; // max allocations allowed per block

		// note: this computed automatically at construction time based on the "_memory_budget_"
		u64                _blocks_size_ = core::dynamic_allocator::min_size_allowed; 
		// max memory this allocator allowed to use/reach
		u64                _memory_budget_ = 0; 
		
		           atomic_u64 _size_ = 0; // current total memory "size of all blocks"
		DEBUG_ONLY atomic_u32 _peak_ = 0;          // peak memory usage
		DEBUG_ONLY atomic_u32 _min_  = 0xFFFFFFFF; // min  memory usage
		
		bool _is_mt_ = false; // is this allocator for multi-threaded usage ?

		g_memory_handle _handle_;

	public:
		// public variables for usage 
		// min/max allowed size for dynamic_allocator "blocks"
		static const u64 min_size_allowed = 16 MB;
		static const u64 max_size_allowed =  2 GB;
		
		// constructor
		dynamic_allocator( core::dynamic_allocator_configs const& parameters ) NOEXP;

		// destructor
		~dynamic_allocator() NOEXP;

		/*
			dynamic_allocator public functions
		*/

		memory_handle allocate(u32 size, memory_tag tag = memory_tag::unkown) NOEXP;
		memory_handle allocate(u32 size, u16 alignement = 0, memory_tag tag = memory_tag::unkown) NOEXP;
		memory_handle allocate(memory_request request) NOEXP;

		// allocate 2 memory chunks next to each other in one call
		memory_handle_2 allocate_tow(memory_request const& request_1 , memory_request const& request_2) NOEXP;
		
		void deallocate(memory_handle handle) NOEXP;

		u64 size() NOEXP; 
		u64 blocks_size() NOEXP; // size of each block
		u32 blocks_count() NOEXP; // return's how many memory block in this allocator

		u64    peak_memory_usage() NOEXP; // max memory usage reached by this allocation
		u64 current_memory_usage() NOEXP; // for all sections
		u64 current_memory_usage(memory_tag section_tag) NOEXP; // for specific section
		
		DEBUG_ONLY string const& name() NOEXP;
		DEBUG_ONLY subsystem_memory_tag tag() NOEXP;

	private: // helper functions
		INLINE u8 add_new_block(u32 block_size) NOEXP;
		// INLINE void remove_block(u8  block_index) NOEXP;

		// note: call this function only from allocate / deallocate
		INLINE void update_size_variables(
			memory_request const& request, memory_handle const& handle , bool increment = true
		) NOEXP;

		INLINE memory_handle allocate_on_st(memory_request const& request) NOEXP;
		INLINE memory_handle allocate_on_mt(memory_request const& request) NOEXP;

		INLINE void deallocate_on_st(memory_handle const& handle) NOEXP;
		INLINE void deallocate_on_mt(memory_handle const& handle) NOEXP;

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