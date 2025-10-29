#pragma once 

#ifndef CORE_CUSTOM_ALLOCATOR
#define CORE_CUSTOM_ALLOCATOR

#include "global_memory.hpp"

namespace core {

	/*
		used to allocate/deallocate global_memory for std library
	*/
	template<class T> struct custom_allocator {

		typedef T value_type;

		 custom_allocator() = default;
		~custom_allocator() = default;

		template<class elm_type> constexpr custom_allocator(
			const custom_allocator <elm_type>&
		) noexcept {

		}

		T* allocate(std::size_t size) {
			return (T*)global_memory::allocate(size * sizeof(T), memory_usage::stdcpp);
		}

		void deallocate(T* pointer, std::size_t size) {
			global_memory::deallocate(pointer);
		}

		template<typename elm_type, typename... Args> void construct(
			elm_type* p, Args&&... args
		) {
			new(p) elm_type(std::forward<Args>(args)...);
		}

		// destructor's caller
		template<typename obj> void destroy(obj* pointer) {
			pointer->~obj();
		}

	}; // struct custom_allocator end

} // core namespace end

#endif