#pragma once 

#ifndef CORE_MEMORY_HPP
#define CORE_MEMORY_HPP

#include <string>

#include "core/macros.hpp"
#include "core/types.hpp"

namespace core {

	namespace memory {

		/*
			note : this tag used to "flag" each memory-allocator for what used for
		*/
		enum class tag : u8 {
			unkown = 0,
			general,
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
		DLL_API void* allocate(size_t size, core::memory::tag memory_tag = core::memory::tag::unkown) noexcept;
		DLL_API void  deallocate(void* pointer) noexcept;

		DLL_API u64 sizeof_allocated() noexcept;
		DLL_API u64 sizeof_section(core::memory::tag memory_tag) noexcept;
		
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
		memory_allocator(memory_allocator&& other) = delete;
		memory_allocator& operator = (const memory_allocator& other) = delete;

	public:	
		// static variables 
		static const u32 min_size_allowed = 1;
		static const u32 max_size_allowed = 512 MB;

		// constructor : destructor
		 memory_allocator(std::string const& name, core::memory::tag memory_tag) noexcept;
		~memory_allocator() = default;

		// basics functions functions

		virtual void* allocate(u32 size)        noexcept;
		virtual void  deallocate(void* pointer) noexcept;

		std::string get_name()       noexcept;
		core::memory::tag get_tag()  noexcept;

	}; 
	// class memory_allocator end

} // core namespace end

#endif
