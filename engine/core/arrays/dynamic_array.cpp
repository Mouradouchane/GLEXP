#pragma once 

#ifndef DYNAMIC_ARRAY_CPP
#define DYNAMIC_ARRAY_CPP

#include "dynamic_array.hpp"

/*
	constructor's
*/ 
template<typename type>	d_array<type>::d_array(u32 elements_count) {
	CRASH_IF(!elements_count, "s_array: 0 size array not allowed !");
	
	// todo: implement + "memory allocation solution"

}

template<typename type>	d_array<type>::d_array( 
	u32 elements_count, 
	std::initializer_list<type> const& elements 
) {
	CRASH_IF(!elements_count, "s_array: 0 size array not allowed !");

	// todo: implement + "memory allocation solution"

}

/*
	destructor
*/ 
template<typename type>	d_array<type>::~d_array() {

	// destroy all array elements
	for (type* ptr = this._start; ptr != this._end; ptr += 1) {
		ptr->~type();
	}

}

/*
	d_array function's
*/

template<typename type>	void d_array<type>::push(type const& new_element) {

	// if resize required
	if ( (this->start + this->_push_index) >= this->_end) {
		this->resize(this->_size);
	}

	// push new element
	*(this->_start + this->_push_index) = new_element;
	 this->_push_index  += 1;

}

template<typename type>	void d_array<type>::resize(u32 elements_count) {

	this->_size += elements_count;
	// todo: resize array using "memory allocator"
	this->_start = nullptr;
	this->_end   = this->_start + this->_size;

	// todo: copy elements to the new memory

}

#endif