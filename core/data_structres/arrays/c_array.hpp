#pragma once 

#ifndef CORE_CARRAY_HPP
#define CORE_CARRAY_HPP

#include <initializer_list>
#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"


namespace core {

	/*
		c-style array as struct contain array variables and few static functions

		NOTE: - c_array is not safe , there's no check no type safety .
		      - designed for full control and access .
	*/
	template<typename type> struct c_array {
		// variables
		u32   size  = NULL;
		type* start = nullptr;
		type* end   = nullptr;
		core::memory_allocator* allocator = nullptr;

		// operator's
		type& operator[](u32 index) {
			return *(this->start + index); 
		}

		/*
			static function's
		*/ 

		static core::c_array<type> create(u32 elements_count, core::memory_allocator* allocator = nullptr) {

		}

		static void fill(core::c_array<type>& _array, type const& fill_value) {

		}

		static void clear(core::c_array<type>& _array) {

		}

		static u64  size(core::c_array<type> const& _array) noexcept {

		}

		static f64 size_of(core::c_array<type> const& _array) noexcept {

		}
		
		static void copy(core::c_array<type> const& source, core::c_array<type>& destination) {
		
		}

		static void move(core::c_array<type>& source, core::c_array<type>& destination) {

		}

		template<typename type> static void sort(
			core::c_array<type>& _array ,
			bool (*compare_function)(type const& a, type const& b)
		) {
			std::sort<type>(_array.start, _array.end, compare_function);
		}

	}; // struct c_array end

} // namespace core end

#endif