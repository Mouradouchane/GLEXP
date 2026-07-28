#pragma once 

#ifndef CORE_MEMORY_REGISTRE_HPP
#define CORE_MEMORY_REGISTRE_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/memory.hpp"

// few errors/warnings for registry
#define REGISTRY_ZERO_SIZE_NOT_ALLOWED "zero size registry is not allowed , auto adjust to default size ."
#define REGISTRY_FAILED_TO_INSERT "registry failed to insert ({},{}) !"
#define REGISTRY_IS_FULL        "registery is full , failed to insert ."
#define REGISTRY_PTR_NOT_FOUND  "registry: pointer {} not found in registry ."

namespace core {
	
	DLL_API_CLASS memory_block;
	
	/*
		registry hash-table for storing allocation's or deallocation's
		note: this registry store allocations + thier sizes and also stroe total size of all current allocations
	*/
	DLL_API_CLASS memory_registry {
	private:
		g_memory_handle    handle;
		memory_allocation* list = nullptr; // registry memory

		u32 size     = 0; // list size in byte
		u32 capacity = 0; // lis capacity

		u32 allocations_count = 0; // how many allocations in register
		u32 allocations_size  = 0; // size of all allocation in register
		
	public:
		static const u32 default_register_capacity = 512; 
		static const u32 register_max_allowed_capacity = 1024;
		// constructor
		memory_registry() NOEXP;
		memory_registry(u32 register_capacity) NOEXP;

		// destructor
		~memory_registry() NOEXP;

		// public functions

		u32 insert(void* ptr, u32 size, memory_tag tag) NOEXP;
		u32 insert(memory_allocation allocation) NOEXP;

		// cut allocation from registry if founded
		memory_allocation cut(u32 index) NOEXP; // fast O(1)
		memory_allocation cut(void* ptr, u32 index) NOEXP; // fast O(1)

		void remove(u32 index) NOEXP;

		// get allocation info
		memory_allocation get_allocation_info(u32 index) NOEXP; // faster O(1)
		// note: O(N) linear search
		memory_allocation_info get_allocation_with_size(u32 target_size) NOEXP; 

		// return capacity if ptr not found
		// note: this could be a linear-search O(N)
		u32 exist(void* ptr) NOEXP; 

		u32 get_size() NOEXP; // registry size in bytes
		u32 get_capacity() NOEXP; // registry capacity

		u32 get_allocations_size()  NOEXP; // total size of all allocation in register
		u32 get_allocations_count() NOEXP; // how many allocation in register

	private: // helper function

		/*
			- this function try to merge contigues allocations
			- this help with "fragmentation" and "provides larger areas" of memory
			- WARNING: this is a expansive operation and locks the entier block !
		*/
		void merge_free_areas() NOEXP;

		u32 hash_pointer(void* ptr) NOEXP;

		INLINE u32 search(u32 start_index = 0, void* ptr = nullptr) NOEXP;

		friend class core::memory_block;
	};
	// class memory_registry end


} // namesapce core end

#endif