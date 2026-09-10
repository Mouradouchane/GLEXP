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

#define CORE_ARRAY_DEFAULT_RESIZE_VALUE 1

namespace core {

/*
	core::array : - is a hybird array you can use it as a fixed size array , or dynamic resizable array .
	              - array resize is manual and not automatic .
				  - needs core::dynamic_allocator to allocate memory .
*/
template<typename type> class array {
private:
	inline static const string _typename_ = TYPE_NAME(type);
	
	core::dynamic_allocator* _allocator_ = nullptr;
	memory_handle             _handle_;

	u64 _size_         = 0;
	u32 _capacity_     = 0;
	u32 _resize_value_ = CORE_ARRAY_DEFAULT_RESIZE_VALUE;
	type* _begin_      = nullptr;
	type* _end_        = nullptr;

#ifdef DEBUG
	memory_tag _tag_ = memory_tag::unkown;
#endif

public:
	/*
		constructor's
	*/
	array() NOEXP = default;
	array(u32 elements_count, core::dynamic_allocator const& _allocator, memory_tag _tag) NOEXP;
	array(core::array<type> const& array_to_copy, core::dynamic_allocator const& _allocator) NOEXP; // copy constructor
	array(core::array<type>& array_to_move) NOEXP; // move constructor 

	/*
		destructor
	*/
	~array() NOEXP;

	/*
		operator's
	*/ 
	type& operator[](u32 const& index) NOEXP;

	/*
		array public functions
	*/
	type* begin() NOEXP;
	type* end()   NOEXP;

	const type* begin() const NOEXP;
	const type* end()   const NOEXP;

	u32  elements_count() NOEXP;
	u64  size()  NOEXP; // size of array in bytes
		
	void clear() NOEXP;
	bool resize() NOEXP;
	void set_resize_value(u32 elements_count);

public:

	/*
		static public functions for array
	*/ 

	// note: copying will discard old elements
	// note: there's no checks or safety against overlapped arrays .
	static void copy_elements(core::array<type> const& source, core::array<type>& destination) NOEXP;
	static void move_elements(core::array<type>& source, core::array<type>& destination) NOEXP;
	static void move_ownership(core::array<type>& source, core::array<type>& destination) NOEXP;
	static void fill(core::array<type>& _array_, type const& fill_value) NOEXP;
	static void sort(core::array<type>& _array_, bool (*compare_function)(type const& a, type const& b) ) NOEXP;
	static void reallocate(core::array<type>& _array_, bool destruct_elements) NOEXP;

}; // class array end


} // namespace core end

#include "array_impl.hpp"

#endif