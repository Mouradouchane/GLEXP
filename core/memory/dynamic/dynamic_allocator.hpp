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

struct free_block {
public:
	u8  index;
	u64 free_memory;
	core::atomic_lock lock;

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
	static const u8 _max_blocks_allowed_ = 128; // max number of possible memory block
	static const u8 _out_range_   = _max_blocks_allowed_;

#ifdef DEBUG
	DEBUG_ONLY string              _name_;
	DEBUG_ONLY subsystem_memory_tag _tag_;

	// used for "debugging purposes" to keep track of memory usage
	DEBUG_ONLY atomic_u32 _sections_[MAX_MEMORY_TAGS] = { 0u };
#endif

	u64 _memory_budget_ = 0; // total memory allocated
	atomic_u64 _budget_ = 0; // used memory

	// memory range
	g_memory_handle _handle_;
	byte* _start_  = nullptr;
	byte* _end_    = nullptr;
	byte* _seek_   = nullptr;

	const u32 _blocks_default_size_ = 4 KB;
	atomic_u8 _blocks_count_ = 0;

	atomic_u8 _insert_index_ = 0;
	core::memory_block _blocks_[ _max_blocks_allowed_ ];
	free_block    _free_blocks_[ _max_blocks_allowed_ ] = { free_block{ _out_range_ , 0 } };

public:
	// public variables for usage 
	static const u64 min_budget_allowed =    4 MB;
	static const u64 max_budget_allowed = 4096 MB;
		
	// constructor
	dynamic_allocator (string const& name, const u64 memory_budget , subsystem_memory_tag tag) NOEXP;

	// destructor
	~dynamic_allocator() NOEXP;

	/*
		dynamic_allocator public functions
	*/

	memory_handle allocate(u32 size, memory_tag tag = memory_tag::unkown) NOEXP;
	memory_handle allocate(u32 size, u16 alignement = 0, memory_tag tag = memory_tag::unkown) NOEXP;
	memory_handle allocate(memory_request request) NOEXP;

	// allocate 2 memory chunks next to each other in one call
	same_pair<memory_handle> allocate_tow(memory_request const& request_1 , memory_request const& request_2) NOEXP;
		
	void deallocate(memory_handle handle) NOEXP;
		
	u32 blocks_count() NOEXP; // return's how many memory block in this allocator

	u64 memory_budget() NOEXP; // size of all memory
	u64 free_memory() NOEXP; 
	u64 current_memory_usage() NOEXP; // for all sections
	u64 current_memory_usage(memory_tag section_tag) NOEXP; // for specific section
		
	DEBUG_ONLY string const& name() NOEXP;
	DEBUG_ONLY subsystem_memory_tag tag() NOEXP;

private: // helper functions

	u8 add_new_block(u32 block_size) NOEXP;
	// INLINE void remove_block(u8  block_index) NOEXP;

	// note: call this function only from allocate / deallocate
	void update_size_variables(
		memory_request const& request, memory_handle const& handle , bool increment = true
	) NOEXP;

	memory_handle allocate_on_st(memory_request const& request) NOEXP;
	memory_handle allocate_on_mt(memory_request const& request) NOEXP;

	void deallocate_on_st(memory_handle const& handle) NOEXP;
	void deallocate_on_mt(memory_handle const& handle) NOEXP;


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