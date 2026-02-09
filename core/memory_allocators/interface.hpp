#pragma once 

#ifndef CORE_INTERFACE_MEMORY_ALLOCATOR_HPP
#define CORE_INTERFACE_MEMORY_ALLOCATOR_HPP 

#include <string>

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"
#include "core/memory_allocators/global/global.hpp"

namespace core {
	
	/*
		interface for using "memory-heap" & "memory-arena" as one type "polymorphism"
	*/
	DLL_API_CLASS memory_allocator {
		protected:
			string _name_ = "";
			memory_usage _section_ = memory_usage::unkown;

		public:
			 memory_allocator() = delete;
			~memory_allocator() = delete;
			
			virtual void* allocate(u32 count) noexcept = 0;
			virtual void  deallocate(void* pointer) noexcept = 0;

			u32 count() noexcept final { return this->size; };
			u32 size()  noexcept final { return this->size; };

			u32 allocated() noexcept;
			u32 available() noexcept;

			string       name() noexcept { return this->_name_; }
			memory_usage section() noexcept { return this->_section_; }
	};

}

#endif