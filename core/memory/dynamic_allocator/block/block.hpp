#pragma once 

#ifndef CORE_MEMORY_BLOCK_HPP
#define CORE_MEMORY_BLOCK_HPP

#include <atomic> // for scoped locks

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/global_allocator/global_allocator.hpp"
#include "core/memory/dynamic_allocator/handles/da_handles.hpp"
#include "core/locks/atomic_lock/atomic_lock.hpp"
#include "core/memory/dynamic_allocator/registery/registery.hpp"

// memory block error's
#define MBE001(CONFIG, REG_SIZE) CORE_ERROR(CONFIG,"memory_block: registry is full max {} allocation !",REG_SIZE);
#define MBE002(CONFIG, MEM_SIZE) CORE_ERROR(CONFIG,"memory_block: is out of memory , failed to allocate {}.", BYTE_TO_STRING(MEM_SIZE));
#define MBE003(CONFIG, MEM_SIZE, MIN, MAX) CORE_ERROR(CONFIG,"memory_block: this size {} is not allowed ! allowed range (min={}, max={}).", BYTE_TO_STRING(MEM_SIZE),BYTE_TO_STRING(MIN),BYTE_TO_STRING(MAX));
#define MBE005(CONFIG) CORE_ERROR(CONFIG,"memory_block: failed to register new allocation !");

struct block_description {
	u64                  size;
	u16                  max_allocations_per_block;
	subsystem_memory_tag tag  DEBUG_ONLY;
};

struct block_status {
	u64                  total_memory;
	u64                  free_memory;
	u64                  allocated_memory;
	u64                  allocations_count;
	subsystem_memory_tag tag  DEBUG_ONLY;
};

namespace core {

DLL_API_CLASS memory_block {

private:
	bool _alive_ = false;
	core::atomic_lock _lock_;

#ifdef DEBUG
	DEBUG_ONLY subsystem_memory_tag _tag_;
#endif

	// memory variables
	memory_handle_g _handle_; 
	byte*  _start_ = nullptr; 
	byte*  _end_   = nullptr; 
	u64    _size_  = 0 ; // size in bytes

	// note: you can use registry functions get info about allocation
	memory_registry _alloc_list_; // holder for memory allocations
	memory_registry _free_list_;  // holder for deallocated memory/areas

public:
	static const u64 min_size_allowed =    4 KB;
	static const u64 max_size_allowed = 4096 MB;
	static const u16 max_allocations_per_block = 1024;

	// constructor
	memory_block() NOEXP = default;
	memory_block(u64 size , u16 max_allowed_allocations , subsystem_memory_tag tag_) NOEXP;

	// destructor
	~memory_block() NOEXP;

	/*
		memory_block public functions
	*/
	void allocate(memory_request const& request, memory_handle& handle) NOEXP;
	void allocate_tow(memory_request_2 const& request, same_pair<memory_handle>& handles) NOEXP;

	bool deallocate(memory_handle& handle) NOEXP;

	bool is_busy()  NOEXP;
	bool is_alive() NOEXP;

	u64 total_memory() NOEXP; // size of memory block in bytes
	u64 free_memory() NOEXP; 
	u64 allocated_memory() NOEXP;

	// use this to query information about allocated memory
	memory_allocation get_allocation_info(memory_handle const& handle) NOEXP;
	void get_status(block_status& obj) NOEXP;

private: // private helper functions

	// this function trigger's merge_free_areas function in free_list .
	// note: this locks the entier block for that process !
	void process_free_list() NOEXP;

	INLINE void handle_registry_2 (
		void** ptr_1, void** ptr_2,
		memory_allocation_info const& allocation, memory_request_2 const& request,
		u32& index_1, u32& index_2
	) NOEXP;

	/*
		not allowed contructor's
	*/
	memory_block(core::memory_block &&     other) = delete;
	memory_block(core::memory_block const& other) = delete;

	/*
		not allowed operator's
	*/
	core::memory_block& operator = (const core::memory_block &&     other) = delete;
	core::memory_block& operator = (const core::memory_block const& other) = delete;

	friend class core::dynamic_allocator;
};
// class memory_block end 

}// namesapce core end


#endif