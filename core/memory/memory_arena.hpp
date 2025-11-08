#pragma once 

#ifndef CORE_MEMORY_ARENA_HPP
#define CORE_MEMORY_ARENA_HPP

/*
	
*/

#include "core/memory/memory.hpp"

// todo: implement memory_arena allocator
namespace core {

	DLL_API_CLASS memory_arena : public core::memory_allocator {

	public :
		memory_arena() { }

		void* allocate(u32 count) override {
			return nullptr;
		};
		void  deallocate(void* pointer) override {

		}
	}; // class memory_arena end

} // namespace core end

#endif