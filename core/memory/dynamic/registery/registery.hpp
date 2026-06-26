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
		core::memory_allocation* list = nullptr; // registry memory

		u32 size     = 0; // list size in byte
		u32 capacity = 0; // lis capacity

		u32 allocations_count = 0; // how many allocations in register
		u32 allocations_size  = 0; // size of all allocation in register
		
		u32 biggest_allocation = 0; // current biggest allocation registerd in

	public:
		static const u32 default_register_capacity = 1024; 
		
		// constructor
		memory_registry() NOEXP;
		memory_registry(u32 register_capacity) NOEXP;

		// destructor
		~memory_registry() NOEXP;

		// registry public functions
		
		u32 insert(void* ptr, u32 size , u8 tag = 0) NOEXP;

		bool remove(void* ptr) NOEXP; // need hashing + look-up
		bool remove(u32 index) NOEXP; // faster O(1)

		// cut allocation from registry and give it back
		// return empty allocation{nullptr,0} if ptr not found
		core::memory_allocation cut(void* ptr) NOEXP;
		// core::memory_allocation cut(u32 index) NOEXP; // faster O(1)

		// return capacity if ptr not found
		u32 exist(void* ptr) NOEXP;

		INLINE u32 get_size() NOEXP; // registry size in bytes
		INLINE u32 get_capacity() NOEXP; // registry capacity

		INLINE u32 get_allocations_size()  NOEXP; // total size of all allocation in register
		INLINE u32 get_allocations_count() NOEXP; // how many allocation in register

		INLINE core::memory_allocation get_info(void* pointer) NOEXP;

		core::i_memory_allocation get_allocation(u32 target_size) NOEXP; // o(n)
		core::i_memory_allocation get_biggest_allocation(u32 target_size) NOEXP; // O(1)

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