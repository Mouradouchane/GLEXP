#pragma once 

#ifndef CORE_GLOBAL_MEMORY_HPP
#define CORE_GLOBAL_MEMORY_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"

namespace core {

	namespace memory {

		/*
			note : each allocator gonna be used in some section/subsystem in engine or core it self,
				   this memory::section enum used to "flag" each memory-allocator for what used for
		*/

		enum class section : u8 {
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


	/*
		- note : this used by other memory-allocators 
		- note : you can use it, but not recommended 
	*/
		DLL_API void* allocate(size_t count, core::memory::section _section_ = section::unkown) noexcept;
		DLL_API void  deallocate(void* pointer) noexcept;

		DLL_API u64 sizeof_allocated() noexcept;
		DLL_API u64 sizeof_section(core::memory::section _section_) noexcept;
		
	} // namespace memory end

	/*
		interface for using "memory-heap" & "memory-arena" as one type "polymorphism"
	*/
	DLL_API_INTERFACE memory_allocator {

	protected:
		string _name_;
		core::memory::section _section_ = core::memory::section::unkown;
		u32 _size_ = 0u;

		byte* start = nullptr;
		byte* end   = nullptr;

	public:
		memory_allocator(string allocator_name, core::memory::section allocator_usage , u32 memory_size) 
			: _name_(allocator_name) , _section_(allocator_usage) , _size_(memory_size) { };
		~memory_allocator() {};

		virtual void* allocate(u32 count)       noexcept = NEED_IMPL;
		virtual void  deallocate(void* pointer) noexcept = NEED_IMPL;

		u32 size() noexcept { return this->_size_; };

		static const string& name(memory_allocator& allocator) noexcept { return allocator._name_; }
		static core::memory::section memory_section(memory_allocator& allocator) noexcept { return allocator._section_; }

	}; 
	// interface memory_allocator end

} // core namespace end

#endif
