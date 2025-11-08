#pragma once 

#ifndef CORE_INTERFACE_MEMORY_ALLOCATOR_HPP
#define CORE_INTERFACE_MEMORy_ALLOCATOR_HPP 

#include "core/macros.hpp"
#include "core/types.hpp"

namespace core {

	DLL_API_CLASS memory_allocator {
		public:
			 memory_allocator() {}
			~memory_allocator() {}
			virtual void* allocate(u32 count) = 0;
			virtual void  deallocate(void* pointer) = 0;
	};

}

#endif