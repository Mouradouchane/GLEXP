#pragma once 

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <initializer_list>
#include "core/types.hpp"
#include "core/memory/memory.hpp"

/*
	class for fixed size array allocated on the heap
	with few vars and functions 
*/
template<typename type> class s_array {
private:
	// note: disabled constructors/operators
	s_array(const s_array& other_array) = delete;
	s_array(s_array&& other_array) = delete;
	s_array& operator=(const s_array& other_array) = delete;
	// ====================================

private:
	u32   _len   = NULL;
	u32   _size  = NULL;
	type* _start = nullptr; // memory start
	type* _end   = nullptr; // memory end

public:
	// public variables for youre personal use :)
	u32 _s = NULL;
	u32 _e = NULL;

	// constructor's / destructor
	 s_array(u32 elements_count);
	 s_array(u32 elements_count, std::initializer_list<type> const& elements);
	~s_array();

	// function's
	type* begin() noexcept;
	type* end()   noexcept;

	type* get(u32 index);
	void  set(u32 index, type const& new_element);

	/*
		note: to call element destructor before remove use destroy
	*/
	void remove(u32 index); 
	void destroy(u32 index); 

	u32 length() noexcept;
	u32 size() noexcept; // elements count
	u64 size_of(void) noexcept; // size in bytes
	f64 size_of(MEMORY_UNIT unit) noexcept; // size in kb,mb,gb
	
	// operator's
	type& operator[](u32 index);

	// few static functions for array
	static void fill(s_array<type>& _array, type const& fill_value) noexcept;
	static void sort(
		s_array<type>& _array,
		bool (*compare_function)(type const& a, type const& b)
	);
};

/*
	c-style like array , in case i need it for some usage
*/
template<typename type> struct c_array {
	u32   size  = NULL;
	type* start = nullptr;
	type* end   = nullptr;  

	/*
		few static function's for c_array
	*/
	static c_array create(u32 elements_count);
	static void    fill(c_array const& _array , type const& fill_value);
	static void    remove(c_array const& _array);
	static void    destroy(c_array const& _array);
	static u64     size_of(c_array const& _array) noexcept;
	static f64     size_of(c_array const& _array , MEMORY_UNIT unit) noexcept;
};

#endif // !ARRAY_HPP