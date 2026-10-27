#pragma once 

#ifndef CORE_MEMORY_BLOCK_HPP
#define CORE_MEMORY_BLOCK_HPP

#include <atomic> // for scoped locks

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/memory.hpp"
#include "core/locks/atomic_lock/atomic_lock.hpp"
#include "core/memory/dynamic/registery/registery.hpp"

#define MEMORY_BLOCK_NAME "memory_block"

#define MEMORY_BLOCK_IS_REGISTRY_FULL     MEMORY_BLOCK_NAME " registry is full max {} allocation !"
#define MEMORY_BLOCK_OUT_OF_MEMORY        MEMORY_BLOCK_NAME " is out of memory , failed to allocate {}bytes ."
#define MEMORY_BLOCK_NOT_ALLOWED_SIZE     MEMORY_BLOCK_NAME " with size={}byte is not allowed because -> min_size_allowed={}, max_size_allowed={} ."
#define MEMORY_BLOCK_OUT_OF_RANGE_POINTER MEMORY_BLOCK_NAME " pointer {} passed to memory_block is out of range , start={} , end={} !"
#define MEMORY_BLOCK_FAILED_TO_REGISTER_ALLOCATION MEMORY_BLOCK_NAME " failed to register new allocation !"

#define MEMORY_HANDLE_BUSY     memory_handle(allocator_response::busy,-1,-1,nullptr)
#define MEMORY_HANDLE_REG_FULL memory_handle(allocator_response::register_full, -1, -1, nullptr)
#define MEMORY_HANDLE_FRAG     memory_handle(allocator_response::fragmeneted, -1, -1, nullptr)

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
	memory_handle allocate(memory_request const& request) NOEXP;
	memory_handle allocate(u32 size_, u32 alignement_, memory_tag tag_, bool wait_for_block = false) NOEXP;

	same_pair<memory_handle> allocate_tow(memory_request_2 const& request) NOEXP;

	void deallocate(memory_handle const& handle) NOEXP;

	bool is_busy()  NOEXP;
	bool is_alive() NOEXP;

	u64 size() NOEXP; // size of block memory in bytes
	u64 free_memory() NOEXP;
	u64 allocated_memory() NOEXP;

	// use this to query information about allocated memory
	memory_allocation get_allocation_info(memory_handle const& handle) NOEXP;

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