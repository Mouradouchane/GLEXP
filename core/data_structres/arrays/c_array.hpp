#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <initializer_list>
#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"


namespace core {

	/*
		c-style array , just a struct contain array variables
	*/
	template<typename type> struct c_array {
		u32   size  = NULL;
		type* start = nullptr;
		type* end   = nullptr;

		/*
			functions for c_array
		*/
		template<typename type> static c_array<type>* create(u32 elements_count);
		template<typename type> static void    fill(c_array<type> const& _array, type const& fill_value);
		template<typename type> static void    remove(c_array<type> const& _array);
		template<typename type> static void    destroy(c_array<type> const& _array);
		template<typename type> static u64     size_of(c_array<type> const& _array) noexcept;
		template<typename type> static f64     size_of(c_array<type> const& _array, memory_unit unit) noexcept;

	}; // struct c_array end

} // namespace core end

#endif