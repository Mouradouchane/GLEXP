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
#define MEMORY_BLOCK_NOT_ALLOWED_SIZE     MEMORY_BLOCK_NAME " with size={}byte is not allowed because -> min_allowed_size={}, max_allowed_size={} ."
#define MEMORY_BLOCK_OUT_OF_RANGE_POINTER MEMORY_BLOCK_NAME " pointer {} passed to memory_block is out of range , start={} , end={} !"
#define MEMORY_BLOCK_FAILED_TO_REGISTER_ALLOCATION MEMORY_BLOCK_NAME " failed to register new allocation !"

namespace core {

DLL_API_CLASS memory_block {

private:
	core::atomic_lock lock;

	g_memory_handle handle;
	byte* start = nullptr; // block start
	byte* end   = nullptr; // block end
	byte* seek  = nullptr; // current free spot
	u64   block_size = 0; // block memory size in bytes

#ifdef DEBUG
	DEBUG_ONLY subsystem_memory_tag block_tag; // memory_block usage
#endif

	/*
		note: registry keep track of allocations count and toal size .
				use registry functions get these information
	*/
	core::memory_registry active_list; // list of the current active/alive allocations in block
	core::memory_registry free_list;   // list of the current free/avalible areas in block

public:
	static const u64 min_allowed_size = 64 KB;
	static const u64 max_allowed_size = 1024 MB;

	// constructor
	memory_block() NOEXP = default;
	memory_block(u64 size , u32 max_allowed_allocations , subsystem_memory_tag tag) NOEXP;

	// destructor
	~memory_block() NOEXP;

	/*
		memory_block public functions
	*/
	memory_handle   allocate(memory_request const& request) NOEXP;
	memory_handle   allocate(u32 size, u32 alignement, memory_tag tag) NOEXP;
	memory_handle_2 allocate_tow(memory_request const& request_1, memory_request const& request_2) NOEXP;

	bool deallocate(memory_handle const& handle) NOEXP;

	bool is_busy() NOEXP;

	u64 size() NOEXP; // size of block memory in bytes
	u64 free_memory() NOEXP;
	u64 allocated_memory() NOEXP;

	// use this to query information about allocated memory
	memory_allocation get_allocation_info(memory_handle const& handle) NOEXP;

private: // private helper functions

	// this function trigger's merge_free_areas function in free_list .
	// note: this locks the entier block for that process !
	void process_free_list() NOEXP;

	// note [WARNING] : lock the block before calling this function !
	INLINE void internal_allocate(memory_request const& request , memory_handle& handle) NOEXP;

	// todo: delete or improve this function
	INLINE u32 handle_registry(
		void** ptr, memory_allocation_info const& allocation , memory_request const& request
	) NOEXP;

	// todo: delete or improve this function
	INLINE void handle_registry_2(
		void** ptr_1,
		void** ptr_2,
		memory_allocation_info const& allocation,
		memory_request const& request_1,
		memory_request const& request_2,
		u32& index_1,
		u32& index_2
	) NOEXP;

	/*
		not allowed contructor's
	*/
	memory_block(core::memory_block&& other) = delete;
	memory_block(core::memory_block const& other) = delete;

	/*
		not allowed operator's
	*/
	core::memory_block& operator = (const core::memory_block&& other) = delete;
	core::memory_block& operator = (const core::memory_block const& other) = delete;

};
// class memory_block end 

}// namesapce core end


#endif