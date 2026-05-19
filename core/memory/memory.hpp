#pragma once 

#ifndef CORE_MEMORY_HPP
#define CORE_MEMORY_HPP

#include <string>

#include "core/macros.hpp"
#include "core/types.hpp"

#define BY_OTHERS  core::memory::management::by_others
#define BY_IT_SELF core::memory::management::by_self

template<typename A, typename B> struct pair_tow_pointers {
	A* ptr_a;
	B* ptr_b;
};

struct tow_pointers {
	void* ptr1;
	void* ptr2;
};

namespace core {

	namespace memory {

		enum class allocator_type : u8 {
			heap = 0,
			pool,
			arena
		};

		/*
			note: use this to tell if object memory is self mananged internally or by some outside system
		*/
		enum class management : u8 {
			by_self = 1, // use this when you have "object mananging his memory by it self"
			by_others    // use this when you have "others mananging object memory"
		};


		/*
			note : this tag used to "flag" each memory-allocator for what used for
		*/
		enum class tag : u8 {
			unkown = 0,
			general,
			event_system,
			work_system,
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
		#ifdef DEBUG
			dev,
		#endif
		};

		enum class unit : u8 {
			byte = 0, kb = 1, mb = 2, gb = 3
		};

		DLL_API std::string tag_to_string(core::memory::tag _tag_);

	/*
		- note : this used by other memory-allocators 
		- note : you can use it, but not recommended 
	*/
		DLL_API void* allocate(size_t size, core::memory::tag memory_tag = core::memory::tag::unkown) NOEXP;
		DLL_API void  deallocate(void* pointer) NOEXP;

		DLL_API u64 sizeof_allocated() NOEXP;
		DLL_API u64 sizeof_section(core::memory::tag memory_tag) NOEXP;
		
	};
	// namespace memory end

	/*
		base class for other memory-allocator !
	*/
	DLL_API_CLASS memory_allocator {

	protected:
		// variables
		std::string _name_;
		core::memory::tag _tag_;

		// disabled contructor's
		memory_allocator(memory_allocator const& other) = delete;
		memory_allocator(memory_allocator &&     other) = delete;
		memory_allocator& operator = (const memory_allocator& other) = delete;

	public:	
		// static variables 
		static const u32 min_size_allowed = 1;
		static const u32 max_size_allowed = 512 MB;

		// constructor : destructor
		 memory_allocator(std::string const& name, core::memory::tag memory_tag) NOEXP;
		~memory_allocator() = default;

		// basics functions functions

		virtual void* allocate(u32 size) NOEXP;
		// used to allocate 2 blocks for memory next to each other
		virtual tow_pointers allocate_tow(u32 size_of_a , u32 size_of_b) NOEXP;

		virtual void deallocate(void* pointer) NOEXP;

		std::string       get_name() NOEXP;
		core::memory::tag get_tag()  NOEXP;

	}; 
	// class memory_allocator end

} // core namespace end

#endif
