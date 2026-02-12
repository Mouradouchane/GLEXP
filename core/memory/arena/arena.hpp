#pragma once 

#ifndef CORE_MEMORY_ARENA_HPP
#define CORE_MEMORY_ARENA_HPP

#include "core/memory/memory.hpp"

namespace core {

	namespace memory {

		// todo: implement memory::arena allocator
		DLL_API_CLASS arena : protected core::memory_allocator {

		public :
			 arena();
			~arena();

			virtual void* allocate(u32 count)        noexcept final;
			virtual void  deallocate(void* pointer)  noexcept final;

			virtual std::string allocator_name()      noexcept final;
			virtual core::memory::tag allocator_tag() noexcept final;

		}; // class arena end
		
	} // namespace memory end

} // namespace core end


#endif