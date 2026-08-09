#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <algorithm>
#include <cstring>
#include <typeinfo>
#include <type_traits>

#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"

namespace core {

/*
	class for fixed count array , array memory allocated on the heap not in stack
*/
template<typename type> class array {

protected:
	core::dynamic_allocator* _allocator_ = nullptr;
	u32   _size_  = 0;
	u32   _count_ = 0;
	type* _begin_ = nullptr;
	type* _end_   = nullptr;
	memory_handle _handle_;

#ifdef DEBUG
	memory_tag _tag_ = memory_tag::unkown;
#endif

public:
	// extra variables for personal use :)
	u32 a = 0, b = 0, c = 0, d = 0;

	/*
		constructor's
	*/
	array() NOEXP = default;
	array(u32 elements_count, core::dynamic_allocator const& _allocator, memory_tag tag_ = memory_tag::unkown) NOEXP;
	array(core::array<type> const& other_array, core::dynamic_allocator const& _allocator) NOEXP; // copy constructor
	array(core::array<type>&& array_to_move) NOEXP; // move constructor 

	/*
		destructor
	*/
	~array() NOEXP;

	/*
		operator's
	*/ 
	type& operator[](u32 index) NOEXP;

	// note: - performe copy operation
	//       - discard old elements
	core::array<type>& operator = (core::array<type> const& array_to_copy) NOEXP;

	// note: - performe move ownership operation
	//       - discard old elements
	core::array<type>& operator = (core::array<type>&& array_to_move) NOEXP;

	/*
		array public functions
	*/
	type& get(u32 index) NOEXP;
	void  set(u32 index, type const& new_element) NOEXP;

	type* begin() NOEXP;
	const type* begin() const NOEXP;

	type* end() NOEXP;
	const type* end() const NOEXP;

	void clear() NOEXP;
	u32  count() NOEXP;
	u32  size()  NOEXP;
		
public:

	/*
		static public functions for array
	*/ 

	static void copy(core::array<type> const& source, core::array<type>& destination) NOEXP;
	static void move(core::array<type>& source, core::array<type>& destination) NOEXP;
	static void fill(core::array<type>& _array, type const& fill_value) NOEXP;
	static void sort(core::array<type>& _array , bool (*compare_function)(type const& a, type const& b) ) NOEXP;
	static void reallocate(core::array<type>& _array , bool destruct_elements) NOEXP;

private:
	/*
		few private helper functions
	*/ 

	static INLINE void deallocate(core::array<type>& _array , bool destruct_elements) NOEXP;
	static INLINE void allocate(core::array<type>& _array , u32 new_elements_count , bool construct_objects = false) NOEXP;
	
}; // class array end

} // namespace core end

#include "array_impl.hpp"

#endif