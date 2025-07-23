#pragma once 

#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <initializer_list>
#include "memory.hpp"
#include "types.hpp"

#include "array.cpp"

template<typename type> class d_array : public s_array<type> {
private:
	// note: disabled constructors/operators
	d_array(const d_array& other_array) = delete;
	d_array(d_array&& other_array) = delete;
	d_array& operator=(const d_array& other_array) = delete;
	// ====================================

private:
	u32 _push_index = nullptr;

public:
	// public variables for youre personal use :)
	u32 s = NULL;
	u32 e = NULL;

	// constructor's / destructor
	 d_array(u32 elements_count);
	 d_array(
		 u32 elements_count, 
		 std::initializer_list<type> const& elements 
	 );
	~d_array();

	// function's
	void push(type const& new_element);
	void resize(u32 elements_count);

};


#endif // !DYNAMIC_ARRAY_HPP