#pragma once 

#ifndef CORE_LOCAL_ARRAY_HPP
#define CORE_LOCAL_ARRAY_HPP

#include <typeinfo>

#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"

#define CORE_ARRAY_DEFAULT_RESIZE_VALUE 16

namespace core {
/*
	core::local_array : - is a hybird array you can use it as a fixed size array , or dynamic resizable array .
	              - array resize is manual and not automatic .
				  - needs core::dynamic_allocator to allocate memory .
*/
template<typename type> class local_array {
private:
	inline static const string _typename_ = TYPE_NAME(type);
	
	memory_handle _handle_;

	u64   _size_         = 0;
	u64   _capacity_     = 0;
	u64   _resize_value_ = CORE_ARRAY_DEFAULT_RESIZE_VALUE;
	type* _begin_        = nullptr;

public:
	/*
		constructor's
	*/
	local_array() NOEXP = default;
	local_array(u32 elements_count, core::dynamic_allocator const& allocator, memory_tag tag) NOEXP;
	local_array(core::local_array<type> const& array_to_copy, core::dynamic_allocator const& allocator) NOEXP; // copy
	local_array(core::local_array<type> const& array_to_copy) NOEXP; // copy
	local_array(core::local_array<type> &&     array_to_move) NOEXP; // move

	/*
		destructor
	*/
	~local_array() NOEXP;

	/*
		operator's
	*/ 
	type& operator[](u32 const& index) NOEXP;

	type& operator= (core::local_array<type> const& array_to_copy) NOEXP; // assing copy
	type& operator= (core::local_array<type> &&     array_to_move) NOEXP; // assing move

	/*
		array public functions
	*/
	type* begin() NOEXP;
	type* end()   NOEXP;

	u32  capacity() NOEXP; // elements count
	u64  size()     NOEXP; // size of array in bytes
		
	void clear()  NOEXP;
	bool resize() NOEXP;
	void set_resize_value(u32 elements_count);

public:

	/*
		static public functions for array
	*/ 

	// note: copying will replace current elements in array
	// note: there's no checks or safety against overlapped arrays .
	static void copy_elements(core::local_array<type> const& source, core::local_array<type>& destination) NOEXP;
	static void move_elements(core::local_array<type>& source, core::local_array<type>& destination) NOEXP;
	static void move_ownership(core::local_array<type>& source, core::local_array<type>& destination) NOEXP;
	static void fill(core::local_array<type>& array_, type const& fill_value) NOEXP;
	static void sort(core::local_array<type>& array_, bool (*compare_function)(type const& a, type const& b) ) NOEXP;
	static void reallocate(core::local_array<type>& array_, bool destruct_elements) NOEXP;

}; // class array end

} // namespace core end

#endif