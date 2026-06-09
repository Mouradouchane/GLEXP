#pragma once 

#ifndef CORE_MEMORY_REGISTRE_HPP
#define CORE_MEMORY_REGISTRE_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/memory.hpp"

namespace core {

	struct memory_allocation {
		void* ptr  = nullptr;
		u32   size = 0;
			
	#ifdef DEBUG
		u8    tag = 0;
	#endif

		// helper function
		INLINE void clear() NOEXP;
	};

	namespace memory {

		/*
			registry hash-table for storing allocation's or deallocation's
			note: this registry store allocations + thier sizes and also stroe total size of all current allocations
		*/
		DLL_API_CLASS registry {
		private:
			core::memory_allocation* list = nullptr; // registry memory

			u32 size     = 0; // list size in byte
			u32 capacity = 0; // lis capacity

			u32 allocations_count = 0; // how many allocations in register
			u32 allocations_size  = 0; // size of all allocation in register
		
		public:
			static const u32 default_register_capacity = 1024; 
		
			// constructor
			registry() NOEXP;
			registry(u32 register_capacity = registry::default_register_capacity) NOEXP;

			// destructor
			~registry() NOEXP;

			// registry public functions

		#ifdef DEBUG  
			// return registry-end if it failed to insert
			u32 insert(void* ptr, u32 size, u8 tag) NOEXP;
		#else 
			u32 insert(void* ptr, u32 size , u8 tag = 0) NOEXP;
		#endif

			bool remove(void* ptr) NOEXP;

			// cut allocation from registry and give it back
			// return empty allocation{nullptr,0} if ptr not found
			core::memory_allocation cut(void* ptr) NOEXP;

			// return capacity if ptr not found
			u32 exist(void* ptr) NOEXP;

			INLINE u32 get_size()  NOEXP; // registry size in bytes
			INLINE u32 get_capacity() NOEXP; // registry capacity

			INLINE u32 get_allocations_size()  NOEXP; // total size of all allocation in register
			INLINE u32 get_allocations_count() NOEXP; // how many allocation in register


		private: // helper function

			// high-performance fibonacci hash function for raw pointers
			u32 hash_pointer(void* ptr) NOEXP;
			INLINE u32 search(u32 start_index = 0, void* ptr = nullptr) NOEXP;
		};
		// struct registry end


	} // namespace memory end


} // namesapce core end

#endif