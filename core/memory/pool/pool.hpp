#pragma once

#ifndef CORE_MEMORY_POOL_HPP
#define CORE_MEMORY_POOL_HPP

/*
	global_memory memory_pool : 
	- fixed count allocation 
	- fixed count memory_pool global_memory pages
	- first fit allocation
*/

#include "core/macros.hpp"
#include "core/memory/memory.hpp"

namespace core {

	namespace memory {

		// todo: implement memory_pool allocator
		DLL_API_CLASS pool : protected core::memory_allocator {

		public:
			 pool();
			~pool();

			void* allocate(u32 count)       noexcept override;
			void  deallocate(void* pointer) noexcept override;

		}; // class memory::pool end

	} // namespace memory end

} // namespace core end


#endif