#pragma once 

#ifndef CORE_MEMORY_BLOCK_HPP
#define CORE_MEMORY_BLOCK_HPP

#include <atomic> // for scoped locks

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/memory.hpp"
#include "core/memory/dynamic/registery/registery.hpp"

#define MEMORY_BLOCK_IS_REGISTRY_FULL "memory_block registry is full max {} allocation !"
#define MEMORY_BLOCK_OUT_OF_MEMORY    "memory_block is out of memory , failed to allocate {}bytes ."
#define MEMORY_BLOCK_NOT_ALLOWED_SIZE "memory block with size={}byte is not allowed because -> min_allowed_size={}, max_allowed_size={} ."
#define MEMORY_BLOCK_INVALID_POINTER  "invalid pointer {} passed to memory_block ! pointer is not in memory block range |{} , {}| ."

namespace core {

	DLL_API_CLASS memory_block {
	private:
		std::atomic<bool> lock;

		byte* start = nullptr; // block start
		byte* end   = nullptr; // block end
		byte* seek  = nullptr; // current free spot
		u64   block_size  = 0; // block memory size in bytes

	#ifdef DEBUG
		u8    block_tag = 0;
	#endif

		/*
			note: registry keep track of allocations count and toal size .
			      use registry functions get these information
		*/
		core::memory_registry active_list; // list of the current active/alive allocations in block
		core::memory_registry free_list;   // list of the current free/avalible areas in block
	
	public:

		static const u64 min_allowed_size =  128 KB;
		static const u64 max_allowed_size = 1024 MB;

		// constructor / destructor
		 memory_block(u32 size , u32 max_allowed_allocations , u8 memory_tag) NOEXP;
		~memory_block() NOEXP;

		// memory_block public functions

		void* allocate(core::memory_request const& request) NOEXP;
		void* allocate(u32 size, u32 alignement = 0, u8 tag = 0) NOEXP;

		bool  deallocate(void* pointer) NOEXP;

		bool is_busy() NOEXP;

		u32 size() NOEXP;
		u32 free_memory() NOEXP;
		u32 allocated_memory() NOEXP;

	private: // private helper functions


		INLINE void handle_registry(
			void** ptr, core::i_memory_allocation const& allocation , core::memory_request const& request
		) NOEXP;

	
		/*
			not allowed contructor's
		*/ 
		memory_block(memory_block & other)      = delete;
		memory_block(memory_block && other)     = delete;
		memory_block(memory_block const& other) = delete;

		/*
			not allowed operator's
		*/ 
		memory_block& operator = (const memory_block & other)      = delete;
		memory_block& operator = (const memory_block && other)     = delete;
		memory_block& operator = (const memory_block const& other) = delete;

	};
	// class memory_block end 
	
}
// namespace core end

#endif