#pragma once 

#ifndef ARRAY_CPP
#define ARRAY_CPP
/*

	note: all definition's in array.hpp
	here only implementation :)

*/
#include "array.hpp"
#include "assert.hpp"

/*
	constructor's
*/ 
template<typename type>	 s_array<type>::s_array(u32 elements_count) {
	CRASH_AT_TRUE(!elements_count, "s_array: 0 size array not allowed !");
	
	// todo: implement + "memory allocation solution"

}

template<typename type>	 s_array<type>::s_array(
	u32 elements_count, std::initializer_list<type> const& elements
) {
	CRASH_AT_TRUE(!elements_count, "s_array: 0 size array not allowed !");
	CRASH_AT_TRUE(elements_count < elements.size(), "s_array: elements count larger than the array !");

	// todo: implement + "memory allocation solution"

}

// destructor
template<typename type>	s_array<type>::~s_array() {

	// destroy all array elements
	for (type* ptr = this._start; ptr != this._end; ptr += 1) {
		ptr->~type();
	}
}

/*
	array function's
*/
template<typename type> type* s_array<type>::begin() noexcept {
	return this->_start;
}

template<typename type> type* s_array<type>::end() noexcept {
	return this->_end;
}

template<typename type> void s_array<type>::insert(u32 index, type const& value) {
	CRASH_AT_TRUE(index >= this->_size, "s_array.insert: index out of array range !");
	this->_len += 1;
	*(this->_start + index) = value;
}

template<typename type> void s_array<type>::remove(u32 index) {
	this->_len = (this->_len > 0) ? this->_len - 1: 0;
	*(this->_start + index) = type();
}

template<typename type> u32 s_array<type>::elements_count() noexcept {
	return this->_len;
}

template<typename type> u32 s_array<type>::size() noexcept {
	return this->_size;
}

template<typename type> f64 s_array<type>::size_of(MEMORY_UNIT unit) noexcept {
	switch (unit) {
		case MEMORY_UNIT::kb: return BYTE_TO_KB(this->_size * sizeof(type));
		case MEMORY_UNIT::mb: return BYTE_TO_MB(this->_size * sizeof(type));
		case MEMORY_UNIT::gb: return BYTE_TO_GB(this->_size * sizeof(type));
		
		default: return f64(this->_size * sizeof(type));
	}
}

/*
	array operator's
*/ 
template<typename type> type& s_array<type>::operator[](u32 index) {
	CRASH_AT_TRUE(index >= this->_size, "s_array[]: index out of array range !");
	
	return *(this->_start + index);
}

/*
	array static function's
*/

template<typename type> static void s_array<type>::fill(
	s_array<type>& _array, type const& fill_value
) noexcept {

	for (type* ptr = _array._start; ptr != _array._end; ptr += 1) {
		*ptr = fill_value;
	}

}

// todo: implement sort
template<typename type> static void s_array<type>::sort(
	s_array<type>& _array,
	bool (*compare_function)(type const& a, type const& b)
) {
	ASSERT_EXP(0, "s_array::sort() is used , but not implement yet !");
}

#endif