#pragma once 

#ifndef CORE_INTERFACE_MEMORY_ALLOCATOR_HPP
#define CORE_INTERFACE_MEMORY_ALLOCATOR_HPP 

#include <string>

#include "core/macros.hpp"
#include "core/types.hpp"

namespace core {
	
	/*
		interface for all "memory-allocators" -> "heap/pool/arena/..." !
		why ? "many-shapes" concept -> to use all allocators as one type .
	*/
	DLL_API_CLASS memory_allocator {
		private:
			std::string name_;

		public:
			/*
			 memory_allocator() {}
			~memory_allocator() {}
			*/
			virtual void* allocate(u32 count) = 0;
			virtual void  deallocate(void* pointer) = 0;

			std::string name() { return this->name_;  }
	};

}

#endif