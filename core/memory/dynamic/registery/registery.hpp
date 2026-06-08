#pragma once 

#ifndef CORE_MEMORY_REGISTRE_HPP
#define CORE_MEMORY_REGISTRE_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/enums.hpp"

namespace core {

	namespace memory {

		struct allocation {
			void* ptr  = nullptr;
			u32   size = 0;
		
		#ifdef DEBUG
			tag   tag_ = tag::unkown;
		#endif

			// helper function
			INLINE void clear() NOEXP;
		};

		/*
			registry hash-table for storing allocation's or deallocation's
			note: this registry store allocations + thier sizes and also stroe total size of all current allocations
		*/
		DLL_API_CLASS registry {
		private:
			core::memory::allocation* list = nullptr; // registry memory
			u32 size     = 0; // size in byte
			u32 count    = 0; // elements count
			u32 capacity = 0; // elements max capacity

			// total size of all current allocations in bytes
			u32 allocations_size = 0; 
		
		public:
			static const u32 default_register_size = 1024; 
		
			// constructor
			registry() NOEXP;
			registry(u32 register_capacity = default_register_size) NOEXP;

			// destructor
			~registry() NOEXP;

			// registry public functions

			// return registry-end if failed to insert
			u32 insert(void* ptr, u32 size , core::memory::tag tag_) NOEXP;

			bool remove(void* ptr) NOEXP;

			// return empty allocation{nullptr,0} if ptr not found
			core::memory::allocation cut(void* ptr) NOEXP;

			// return capacity if ptr not found
			u32 exist(void* ptr) NOEXP;

			INLINE u32 get_size() NOEXP;
			INLINE u32 get_capacity() NOEXP;
			INLINE u32 get_allocations_size()  NOEXP;
			INLINE u32 get_allocations_count() NOEXP;


		private: // helper function

			// high-performance fibonacci hash function for raw pointers
			u32 hash_pointer(void* ptr) NOEXP;
			INLINE u32 search(u32 start_index = 0, void* ptr = nullptr) NOEXP;
		};
		// struct registry end

	}

}
#endif