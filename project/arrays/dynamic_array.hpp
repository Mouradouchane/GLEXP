#pragma once 

#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <initializer_list>
#include "memory.hpp"
#include "types.hpp"

template<typename type> class d_array {
private:
	// note: disabled constructors/operators
	d_array(const d_array& other_array) = delete;
	d_array(d_array&& other_array) = delete;
	d_array& operator=(const d_array& other_array) = delete;
	// ====================================

private:
	u32   _size  = NULL;
	u32   _len   = NULL;
	type* _start = nullptr; // memory start
	type* _end   = nullptr; // memory end

public:
	// public variables for youre personal use :)
	u32 s = NULL;
	u32 e = NULL;

};


#endif // !DYNAMIC_ARRAY_HPP