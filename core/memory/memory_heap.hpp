/*
	global_memory memory_heap :
	- linear allocation
	- support big size allocations
	- multi global_memory pages with differnet sizes
*/
#pragma once

#ifndef MEMORY_HEAP_HPP
#define MEMORY_HEAP_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "memory.hpp"

struct registry_pair {
	void* pointer = nullptr;
	u32   size    = NULL;
};

namespace core {

	class memory_heap {

	private:
		// memory_heap static variables
		static const u32 minimum_heap_size_allowed = KB_TO_BYTE(1 KB);
		static const u32 maximum_heap_size_allowed = MB_TO_BYTE(512 MB);

		/*
			copy/move constructor's and operator's disabled
		*/
		memory_heap(memory_heap const& other) = delete;
		memory_heap(memory_heap&& other) = delete;
		memory_heap& operator = (const memory_heap& other) = delete;

	public:
		// memory_heap public static functions
		static f32 minimum_size_allowed(memory_unit return_value_unit) noexcept;
		static u32 maximum_size_allowed(memory_unit return_value_unit) noexcept;

	private:
		// memory_heap type/usage
		memory_usage section = memory_usage::unkown;

		// memory_heap global_memory variables
		u32 max_allowed_allocations = 1000;
		u32 alloc_size = NULL;
		u32 heap_size = NULL;

		byte* start = nullptr;
		byte* end = nullptr;
		byte* seek = nullptr; // last deallocate position

		u32 registered = NULL;

		// open addressing hash-table
		u32 alloc_list_size = NULL;
		registry_pair* alloc_list = nullptr;

		// sorted list
		u32 free_list_range = 1;
		registry_pair* free_list = nullptr;

	public:
		// constructor / destructor
		memory_heap(
			u32 heap_size,
			u32 max_allocation = 1000,
			memory_usage heap_usage = memory_usage::unkown
		);
		~memory_heap();

		// memory_heap public functions
		void* allocate(u32 size);
		void  deallocate(void* pointer);

		u32 size() noexcept;
		f32 size(memory_unit return_value_unit) noexcept;

		u32 allocated(memory_unit return_value_unit) noexcept;
		u32 available(memory_unit return_value_unit) noexcept;

	private:
		// memory_heap private functions
		inline void merge_free_areas();
		u32 hash_pointer(void* pointer);

		inline void register_allocation(void* pointer, u32 size);
		inline void allocate_from_free_list(void** pointer, u32 size, u32 index);
		inline void unregister_allocation(u32 _index);
		// this function search the free_list looking for empty spot
		inline void find_free_location(u32& index_output, u32 size__);

		// sort from bigger to smaller
		inline void sort_list_by_size(registry_pair* list, u32 size);
		inline void sort_list_by_address(registry_pair* list, u32 size);
		inline void init_registry_list(registry_pair* list, u32 size);
	};

}

#endif