#pragma once 

#ifndef CORE_STD_ALLOCATOR
#define CORE_STD_ALLOCATOR

#include "core/memory/memory.hpp"

/*
	note: maybe this need to get delete !!!
*/

namespace core {

	/*
		used for/by std:: library allocate/deallocate memory for objects
	*/
	template<class T> struct std_allocator {

		typedef T value_type;

		 std_allocator() = default;
		~std_allocator() = default;

		template<class elm_type> constexpr std_allocator(
			const std_allocator <elm_type>&
		) noexcept {

		}

		T* allocate(std::size_t count) {
			return (T*)core::memory::allocate(count * sizeof(T), core::memory::tag::stdcpp);
		}

		void deallocate(T* pointer, std::size_t count) {
			core::memory::deallocate(pointer);
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

	}; // struct std_allocator end

} // core namespace end

#endif