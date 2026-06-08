#pragma once 

#ifndef CORE_MEMORY_BLOCK_HPP
#define CORE_MEMORY_BLOCK_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/dynamic/registery/registery.hpp"

#include "libs/mimalloc/mimalloc.h"
#include "core/memory/memory.hpp"

namespace core {

	struct memory_block {

		byte* start = nullptr; // block start
		byte* end   = nullptr; // block end
		byte* seek  = nullptr; // current free spot
		u64   size  = 0;       // block memory size in bytes

		mi_arena_id_t arena_id;
		mi_heap_t*    heap;

		/*
			note: registry manange total sizes both lists ,
					use allocations_size() for that
		
		registry active_list; // list of the current active/alive allocations in block
		registry free_list;   // list of the current free/avalible areas in block
		*/

		// constructor / destructor
		 memory_block(u32 size) NOEXP;
		~memory_block() NOEXP;

		// block public functions
		void* allocate(u32 size) NOEXP;
		void  deallocate(void* pointer) NOEXP;

		u32 sizeof_all() NOEXP;
		u32 sizeof_free()  NOEXP;
		u32 sizeof_allocated() NOEXP;


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
		INLINE void sort_by_pointer(memory::allocation* list, u32 length) NOEXP;

	};
	// struct block end 
	

}
// namespace core end

#endif