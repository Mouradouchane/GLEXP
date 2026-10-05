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

struct block_description {
	u64 size;
	u16 max_allocations;
	subsystem_memory_tag tag;
};

struct block_status {
	u64 size;
	u64 free_memory;
	u16 allocations_count;
};

namespace core {

/*
	- dynamic_allocator handle dynamic memory allocation with different size's like new/malloc .
	- note: this allocator run on limited memory budget , if run's out of memory you get "nullptr" or "crash" .
	- note: designed to be capable of multi-threaded allocations !

	- how it's work ?
		- the allocator loops through free_blocks list and lock for avalible block ,
		  then "locks" that block to performe the allocation .
		- by having multiple blocks we can handle multiple allocations at the sametime without much blocking .
*/
DLL_API_CLASS dynamic_allocator {
private:

#ifdef DEBUG
	DEBUG_ONLY subsystem_memory_tag _tag_;
	DEBUG_ONLY string _name_;

	// used for "debugging purposes" to keep track of memory usage
	DEBUG_ONLY atomic_u32 _sections_[MAX_MEMORY_TAGS] = { 0u };
#endif

	g_memory_handle _handle_;

	u64 _memory_budget_ = 0; // total memory allocated

	core::memory_block* _blocks_ = nullptr;
	atomic_u8           _blocks_count_ = 0;


public:
	// public variables for usage 
	static const u64 min_budget_allowed =   64 KB;
	static const u64 max_budget_allowed = 4096 MB;
		
	// constructor
	dynamic_allocator(string const& name, const u64 blocks_size , const u8 blocks_count, const subsystem_memory_tag tag) NOEXP;
	dynamic_allocator(string const& name, block_description* blocks, const u8 blocks_count, const subsystem_memory_tag tag) NOEXP;

	// destructor
	~dynamic_allocator() NOEXP;

	/*
		dynamic_allocator public functions
	*/

	memory_handle allocate(u32 size, memory_tag tag, u8 block_index, bool wait_for_block) NOEXP;
	memory_handle allocate(u32 size, u16 alignement, memory_tag tag, u8 block_index, bool wait_for_block) NOEXP;
	memory_handle allocate(memory_request request, u8 block_index, bool wait_for_block) NOEXP;

	// allocate 2 memory chunks next to each other in one call
	same_pair<memory_handle> allocate_tow(memory_request const& request_1, memory_request const& request_2, u8 block_index, bool wait_for_block) NOEXP;
		
	void deallocate(memory_handle handle) NOEXP;
		
	u32 blocks_count() NOEXP;

	block_status get_block_status(u8 block_index) NOEXP; // retrun few info about a memory_block

	u64 memory_budget() NOEXP; // size of all blocks
	u64 free_memory() NOEXP;  // free memory in all blocks

	u64 current_memory_usage() NOEXP; // for all sections
	u64 current_memory_usage(memory_tag section_tag) NOEXP; // for specific section
		
	DEBUG_ONLY string const& name() NOEXP;
	DEBUG_ONLY subsystem_memory_tag tag() NOEXP;

private: // helper functions

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