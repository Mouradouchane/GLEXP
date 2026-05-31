#pragma once 

#ifndef CORE_dynamic_allocator_HPP
#define CORE_dynamic_allocator_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"


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

		struct allocation {
			void* ptr;
			u64   size;
		};

		// hash-table for storing allocations
		struct registry {
			allocation* list;
			u32         size;

			// constructor / destructor
			 registry(void* memory , u32 size) NOEXP;
			~registry( ) NOEXP;

			INLINE bool insert(void* ptr, u64 size) NOEXP;
			INLINE bool remove(void* ptr) NOEXP;

		private: // helper function
			u32 hash(u32 key) NOEXP;
		};

		struct block {
			void* start = nullptr; // block start
			byte* end   = nullptr; // block end
			byte* seek  = nullptr; // current free spot
			u64   size  = 0;       // block size in bytes
			u64   free  = 0;       // current free memory in block

			memory::registry list; // list of the current allocations in block
			memory::registry free; // list of the current free  areas in block

			// constructor / destructor
			 block(u64 size) NOEXP;
			~block() NOEXP;

			// block public functions
			void* allocate(u32 size) NOEXP;
			void  deallocate(void* pointer) NOEXP;
		};


		DLL_API_CLASS dynamic_allocator {
		private:
			memory::block *blocks;
			u32            count; // current blocks count
			
		#ifdef DEBUG // debug-only variables
			STRING      name; 
			memory::tag tag;
		#endif

			// not allowed contructor's
			dynamic_allocator(dynamic_allocator& other) = delete;
			dynamic_allocator(dynamic_allocator&& other) = delete;
			dynamic_allocator(dynamic_allocator const& other) = delete;

			// not allowed operator's
			dynamic_allocator& operator = (const dynamic_allocator& other) = delete;
			dynamic_allocator& operator = (const dynamic_allocator&& other) = delete;
			dynamic_allocator& operator = (const dynamic_allocator const& other) = delete;

		public:
			// min/max allowed size for dynamic_allocator
			static const u32 min_size_allowed = 1 KB;
			static const u32 max_size_allowed = 512 MB;

			// constructor
		#ifdef DEBUG
			dynamic_allocator(STRING const& name, memory::tag tag, u32 blocks_size) NOEXP;
		#else
			dynamic_allocator(u32 blocks_size) NOEXP;
		#endif

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
		#endif

			u64 sizeof_all() NOEXP;
			u64 sizeof_avalible() NOEXP;
			u64 sizeof_allocated() NOEXP;
		};
		// class allocator end


	}
	// namesapce memory end

} // namesapce core end


#endif