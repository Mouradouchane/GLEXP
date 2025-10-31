#pragma once

#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

/*
	global_memory memory_pool : 
	- fixed size allocation 
	- fixed size memory_pool global_memory pages
	- first fit allocation
*/

#include "custom_allocator.hpp"
#include "memory.hpp"

namespace core {

	// todo: implement memory_pool allocator
	DLL_API_CLASS memory_pool : public core::memory_allocator {

	public:
		memory_pool() {};


		void* allocate(u32 size) override {
			return nullptr;
		};
		void  deallocate(void* pointer) override {
			
		}

	}; // class memory_pool end

} // namespace core end

#endif