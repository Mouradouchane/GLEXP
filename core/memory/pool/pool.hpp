#pragma once

#ifndef CORE_MEMORY_POOL_HPP
#define CORE_MEMORY_POOL_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include "core/memory/memory.hpp"

namespace core {


	/*
		memory pool is fixed size allocator , also fast for allocate/deallocate memory
	*/

	DLL_API_CLASS memory_pool {
	private:
		byte* start = nullptr;
		byte* end   = nullptr;
		byte* seek  = nullptr; 

		u32 pool_size  = 0; // size of pool in bytes

		u32 allocations_count = 0; 
		u32 allocations_size  = 0;
		
		u8 pool_tag = 0; // pool used for what

	public:
		// constructor's
		 memory_pool(u16 element_size , u32 elements_count , u8 usage_tag);

		// destructor
		~memory_pool();

		// pool public functions
		index32 allocate() NOEXP;
		void deallocate(index32 index) NOEXP;

		INLINE void* get_pointer(index32 index) NOEXP;

	}; // class memory_pool end
	
} // namespace core end


#endif