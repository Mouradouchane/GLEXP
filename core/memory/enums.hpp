#pragma once 

#ifndef CORE_MEMORY_ENUMS_HPP
#define CORE_MEMORY_ENUMS_HPP

#include "core/types.hpp"

/*

	here all the enums for memory allocators

*/

namespace core {

	namespace memory {

		/*
			debug-only option , used to flag the allocation used for what
		*/
		enum class tag : u8 {
			unkown = 0,

			general,
			memory,
			event,
			work_system,
			entity_system,
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
			configs,

			#ifdef DEBUG
				dev,
			#endif
		};


		// used with void* allocator to tell the allocator real type
		enum class allocator_type : u8 {
			unkown = 0,
			dynamic_allocator = 1,
			pool_allocator,
			arena_allocator
		};


	}

} 

#endif