#pragma once 

#ifndef CORE_MEMORY_BLOCK_HPP
#define CORE_MEMORY_BLOCK_HPP

#include <atomic>

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/memory.hpp"
#include "core/memory/dynamic/registery/registery.hpp"


namespace core {

	DLL_API_CLASS memory_block {
	private:
		std::atomic<bool> lock = false;

		byte* start = nullptr; // block start
		byte* end   = nullptr; // block end
		byte* seek  = nullptr; // current free spot
		u64   block_size  = 0; // block memory size in bytes
		
	#ifdef DEBUG
		u8    tag = 0; // block tag 
	#endif

		/*
			note: registry keep track of allocations count and toal size .
			      use registry functions get these information
		*/
		core::memory::registry active_list; // list of the current active/alive allocations in block
		core::memory::registry free_list;   // list of the current free/avalible areas in block
	
	public:
		static const u64 min_allowed_size =  128 KB;
		static const u64 max_allowed_size = 1024 MB;

		// constructor / destructor
		 memory_block(u32 size , u32 max_allowed_allocations , u8 memory_tag) NOEXP;
		~memory_block() NOEXP;

		// memory_block public functions

	#ifdef DEBUG
		void* allocate(u32 size, u8 tag) NOEXP;
	#else 
		void* allocate(u32 size, u8 tag = 0) NOEXP;
	#endif

		void* allocate(core::memory_request const& request) NOEXP;

		void  deallocate(void* pointer) NOEXP;

		u32 size() NOEXP;
		u32 free_memory()  NOEXP;
		u32 allocated_memory() NOEXP;


	private: // private helper functions

		/*
			- this function try to merge free spots in block
			- this help to avoid "memory fragmentation" and "help providing larger spots" of memory

			WARNING: this is a expansive operation and locks the entier block
		*/
		INLINE void merge_free_areas() NOEXP;

		INLINE void allocate_from_free_list(void** pointer, u32 count, u32 index) NOEXP;

		// this function search the free_list looking for empty spot
		INLINE void find_free_location(u32& index_output, u32 size_) NOEXP;

		// sort from bigger to smaller
		INLINE void sort_by_pointer(core::memory_allocation* list, u32 length) NOEXP;


		// not allowed contructor's
		memory_block(memory_block & other)      = delete;
		memory_block(memory_block && other)     = delete;
		memory_block(memory_block const& other) = delete;

		// not allowed operator's
		memory_block& operator = (const memory_block & other)      = delete;
		memory_block& operator = (const memory_block && other)     = delete;
		memory_block& operator = (const memory_block const& other) = delete;

	};
	// class memory_block end 
	
}
// namespace core end

#endif