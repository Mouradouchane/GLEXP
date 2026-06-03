#pragma once 

#ifndef CORE_dynamic_allocator_HPP
#define CORE_dynamic_allocator_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"

/*
	memory allocator's errors/warnings list
*/
#define ZERO_SIZE_MEMORY            "0 size memory allocation is not allowed ."
#define ZERO_SIZE_ALLOCATION        "0 size allocation is not allowed ."
#define TO_BIG_ALLOCATION           "allocation with size={} , is bigger than allocator size={} ."
#define NOT_ALLOWED_ALLOCATION_LIST "allocation-list size={} , is not in allowed range min={} , max={} ."
#define NOT_ALLOWED_ALLOCATOR_SIZE  "memory allocator size={} , not in allowed range min={} , max={} ."
#define MEMORY_ALLOCATOR_IS_FULL    "memory allocator with name={} reach his maximunm allocations ({}/{}) ."


namespace core {

	namespace memory {
		
		/*
			note : this tag used to "flag" each memory-allocator for what used for
		*/
		enum class tag : u8 {
			unkown = 0,
			general,
			event_system,
			work_system,
			assets,
			gui,
			graphics,
			physics,
			collision,
			animations,
			ai,
			audio,
			stdcpp,
			core,
			files,
		#ifdef DEBUG
			dev,
		#endif
		};
		// ============ enum tag end ===============


		struct allocation {
			void* ptr  = nullptr;
			u32   size = 0;
		};


		/*
			registry hash-table for storing allocation's or deallocation's
			note: this registry store allocations + thier sizes and also stroe total size of all current allocations
		*/ 
		struct registry {
			static const u32 default_registry_size = 1000;
		
			memory::allocation* list; // registry memory
			u32 registry_size = 0; 

			u32 allocs_count = 0;
			u32 allocs_size  = 0;
		
			// constructor / destructor
			 registry(void* memory , u32 count) NOEXP;
			~registry( ) NOEXP;

			u32  insert(void* ptr, u32 size) NOEXP;
			bool remove(void* ptr) NOEXP;

			// return empty allocation{nullptr,0} if ptr not found
			memory::allocation cut(void* ptr) NOEXP;

			// return -1 if ptr not found
			INLINE i64 exist(void* ptr) NOEXP;

			INLINE u32 size() NOEXP;
			INLINE u32 allocations_count() NOEXP;
			INLINE u32 allocations_size()  NOEXP;

		private: // helper function
			u32 hash_ptr(void* ptr) NOEXP;

		};
		// ========== struct registry end =============


		struct block {
			void* start = nullptr; // block start
			byte* end   = nullptr; // block end
			byte* seek  = nullptr; // current free spot
			u64   size  = 0;       // block size in bytes
			
			/*
				note: registry manange total sizes both lists ,
				      use allocations_size() for that
			*/
			memory::registry active_list; // list of the current active/alive allocations in block
			memory::registry free_list;   // list of the current free/avalible areas in block

			// constructor / destructor
			 block(u32 size) NOEXP;
			~block() NOEXP;

			// block public functions
			void* allocate(u32 size) NOEXP;
			void  deallocate(void* pointer) NOEXP;
		
			u32 sizeof_all() NOEXP;
			u32 sizeof_free()  NOEXP;
			u32 sizeof_allocated() NOEXP;


		private : // private helper functions

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
		// ========== struct block end ============


		DLL_API_CLASS dynamic_allocator {
		private:
			memory::block *blocks;
			u32            count; // current blocks count
			
		#ifdef DEBUG // debug-only variables
			STRING      name; 
			memory::tag tag;
		#endif

			// not allowed contructor's
			dynamic_allocator() = delete;
			dynamic_allocator(dynamic_allocator&  other)      = delete;
			dynamic_allocator(dynamic_allocator&& other)      = delete;
			dynamic_allocator(dynamic_allocator const& other) = delete;

			// not allowed operator's
			dynamic_allocator& operator = (const dynamic_allocator&  other)      = delete;
			dynamic_allocator& operator = (const dynamic_allocator&& other)      = delete;
			dynamic_allocator& operator = (const dynamic_allocator const& other) = delete;

		public:
			// min/max allowed size for dynamic_allocator
			static const u32 min_size_allowed =  64 KB;
			static const u32 max_size_allowed = 512 MB;

			// constructor
			dynamic_allocator( STRING const& allocator_name, memory::tag usage_tag, u32 memory_blocks_size ) NOEXP;

			// destructor 
			~dynamic_allocator() NOEXP;

			// dynamic_allocator public functions
			void* allocate(u32 size) NOEXP;
			void  deallocate(void* pointer) NOEXP;

			// tow memory-allocations in one call
			tow_pointers allocate_tow(u32 size_of_first, u32 size_of_second) NOEXP;

		#ifdef DEBUG
			STRING       get_name() NOEXP;
			memory::tag  get_tag()  NOEXP;
		#else 
			INLINE STRING       get_name() NOEXP;
			INLINE memory::tag  get_tag()  NOEXP;
		#endif

			u32 sizeof_all()       NOEXP;
			u32 sizeof_avalible()  NOEXP;
			u32 sizeof_allocated() NOEXP;

		};
		// class allocator end


	}
	// namesapce memory end

} // namesapce core end


#endif