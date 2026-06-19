#pragma once 

#ifndef CORE_MEMORY_ARENA_HPP
#define CORE_MEMORY_ARENA_HPP

#include "core/memory/memory.hpp"

namespace core {

	DLL_API_CLASS memory_arena {
	private:

	public :
		// constructor/destructor
		 memory_arena(core::g_memory_request const& arean_parameters);
		~memory_arena();

		// public functions
		void* allocate(u32 size , u8 tag) NOEXP;
		void  deallocate(void* pointer)   NOEXP;

		DEBUG_ONLY std::string arena_name() NOEXP;
		DEBUG_ONLY u8          arean_tag()  NOEXP;
		DEBUG_ONLY u32         arena_size() NOEXP;

	}; // class memory_arena end
		

} // namespace core end


#endif