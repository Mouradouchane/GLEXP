/*
	 core::memory::heap goals :
	- linear allocation
	- support big count allocations
	- multi global_memory pages with differnet sizes
*/
#pragma once

#ifndef CORE_MEMORY_HEAP_HPP
#define CORE_MEMORY_HEAP_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/STRINGs/STRING.hpp"
#include "core/memory/memory.hpp"

struct registry_pair {
	void* pointer  = nullptr;
	u32   count    = NULL;
};

namespace core {

	namespace memory {

		DLL_API_CLASS heap : protected core::memory_allocator {

		private:
			// copy/move constructor's and operator's disabled
			heap(heap const& other) = delete;
			heap(heap&& other) = delete;
			heap& operator = (const heap& other) = delete;

		protected:
			// heap variables
			byte* start = nullptr;
			byte* end   = nullptr;
			byte* seek  = nullptr;
			u32  _size_ = 0u;

			u32 max_allowed_allocations = 1000u;
			u32 alloc_size = NULL;
			u32 registered = NULL;

			// open addressing hash-table
			u32 alloc_list_size = NULL;
			registry_pair* alloc_list = nullptr;

			// sorted list
			u32 free_list_range = 1;
			registry_pair* free_list = nullptr;

		public:
			// static variables 
			static const u32 min_size_allowed = 1 KB;
			static const u32 max_size_allowed = 512 MB;

			// constructor / destructor
			heap(
				STRING const& heap_name, core::memory::tag memory_tag, u32 heap_size, u32 max_allocation = 1000u
			) noexcept;
			~heap();

			// memory_heap public functions
			void* allocate(u32 count) noexcept override final;
			void  deallocate(void* pointer) noexcept override final;

			u32 size() noexcept;

			u32 allocated() noexcept;
			u32 available() noexcept;

		private:
			// memory_heap private functions
			inline void merge_free_areas();
			u32 hash_pointer(void* pointer);

			inline void register_allocation(void* pointer, u32 count);
			inline void allocate_from_free_list(void** pointer, u32 count, u32 index);
			inline void unregister_allocation(u32 _index);
			// this function search the free_list looking for empty spot
			inline void find_free_location(u32& index_output, u32 size__);

			// sort from bigger to smaller
			inline void sort_list_by_size(registry_pair* list, u32 count);
			inline void sort_list_by_address(registry_pair* list, u32 count);
			inline void init_registry_list(registry_pair* list, u32 count);

		}; // class heap end
	
	} // namespace memory end

} // namespace core end

#endif