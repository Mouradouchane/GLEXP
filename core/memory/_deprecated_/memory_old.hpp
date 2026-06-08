#pragma once 

#ifndef CORE_MEMORY_ALLOCATOR_HPP
#define CORE_MEMORY_ALLOCATOR_HPP

#include <string>

#include "core/macros.hpp"
#include "core/types.hpp"

/*
	memory allocator's errors/warnings list 
*/
#define ZERO_SIZE_MEMORY            "0 size memory allocation is not allowed ."
#define ZERO_SIZE_ALLOCATION        "0 size allocation is not allowed ."
#define TO_BIG_ALLOCATION           "allocation with size={} , is bigger than allocator size={} ."
#define NOT_ALLOWED_ALLOCATION_LIST "allocation-list size={} , is not in allowed range min={} , max={} ."
#define NOT_ALLOWED_ALLOCATOR_SIZE  "memory allocator size={} , not in allowed range min={} , max={} ."
#define MEMORY_ALLOCATOR_IS_FULL    "memory allocator with name={} reach his maximunm allocations ({}/{}) ."

template<typename first_ptr_type, typename second_ptr_type> struct duo_pointers {
	 first_ptr_type* first;
	second_ptr_type* second;
};

struct tow_pointers {
	void* ptr1;
	void* ptr2;
};


namespace core {

	/*
		global memory-allocator
	*/
	namespace memory {


		DLL_API std::string tag_to_string(core::memory::tag _tag_);

	/*
		- note : this used by other memory-allocators 
		- note : you can use it, but not recommended 
	*/
		DLL_API void* allocate(size_t size, core::memory::tag memory_tag) NOEXP;
		DLL_API void  deallocate(void* pointer) NOEXP;

		DLL_API u64 sizeof_allocated() NOEXP;
		DLL_API u64 sizeof_section(core::memory::tag memory_tag) NOEXP;
		
	};
	// namespace memory end


	/*
		class memory_allocator like malloc/new heap-allocators but fixed size
		note: memory_allocator will crash when it pass the max capacity !!!
	*/
	DLL_API_CLASS memory_allocator {

	protected:
		// allocator variables
		std::string       _name_;
		core::memory::tag  _tag_;

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

		// not allowed contructor's
		memory_allocator(memory_allocator &      other) = delete;
		memory_allocator(memory_allocator &&     other) = delete;
		memory_allocator(memory_allocator const& other) = delete;

		// not allowed operator's
		memory_allocator& operator = (const memory_allocator &       other) = delete;
		memory_allocator& operator = (const memory_allocator &&      other) = delete;
		memory_allocator& operator = (const memory_allocator const&  other) = delete;

	public:	
		// min/max allowed size for memory_allocator
		static const u32 min_size_allowed =   1 KB;
		static const u32 max_size_allowed = 512 MB;

		// constructor/destructor
		 memory_allocator(std::string const& name, core::memory::config const& allocator_configs) NOEXP;
		~memory_allocator() NOEXP;

		// public functions

		template<typename type> 
		type* allocate(u32 size) NOEXP;

		void* allocate(u32 size) NOEXP;

		// allocate_tow : allocate 2 chunks of memory next to each other in one call
		template<typename _first_, typename _second_> 
		duo_pointers<_first_, _second_> allocate_tow(u32 size_of_first, u32 size_of_second) NOEXP;

		tow_pointers allocate_tow(u32 size_of_first, u32 size_of_second) NOEXP;

		void deallocate(void* pointer) NOEXP;

		std::string       get_name() NOEXP;
		core::memory::tag get_tag()  NOEXP;


	private: // allocator private helper functions

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

	}; 
	// class memory_allocator end

} // core namespace end

#endif
