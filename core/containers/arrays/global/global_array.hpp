#pragma once 

#ifndef CORE_GLOBAL_ARRAY_HPP
#define CORE_GLOBAL_ARRAY_HPP

#include "core/types.hpp"
#include "core/memory/memory.hpp"

namespace core {
/*
	global_array for simple and gloabl usage , used with global memory allocator core::memory .
	NOTE: there is no safety use it carefuly .
*/
template<typename type> class global_array {
	private:
		inline static const string _typename_ = TYPE_NAME(type);

		memory_handle_g _handle_;
		u64   _capacity_ = 0;
		u64   _size_     = 0;
		type* _begin_    = nullptr;

	public:
		// constructor
		global_array( ) = default;
		global_array(global_array<type> const& array_to_copy) NOEXP;
		global_array(global_array<type> &&     array_to_move) NOEXP;
		global_array(const u32 elements_count, subsystem_memory_tag tag) NOEXP;

		// operators
		type& operator[] (u32 index) NOEXP;
		type& operator= (global_array<type> const& array_to_copy) NOEXP; 
		// note: move ownership not elements
		type& operator= (global_array<type> && array_to_move) NOEXP; 
		
		// destructor
		~global_array();
		
		// functions
		u64   capacity() NOEXP;
		u64   size()     NOEXP;
		
		type* begin()    NOEXP;
		type* end()      NOEXP;

		subsystem_memory_tag tag() NOEXP;

	}; // calss global_array end

} // namespace core end

#endif