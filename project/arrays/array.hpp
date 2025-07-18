#pragma once 

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "types.hpp"

template<typename type, class allocator> class s_array {

private:
	u32   _size = NULL;
	u32   _len  = NULL;
	type*   arr = nullptr;

public:
	// public variables for youre personal use :)
	u32 s = NULL;
	u32 e = NULL;

	// constructor's / destructor
	 s_array(u32 size);
	 s_array(u32 size, type const& ...elements);
	~s_array();

	// function's
	void fill(type const& value) noexcept;
	void clean( ) noexcept;

	type* pointer() noexcept;

	// operator's
	type& operator[](u32 index);
};

#endif // !ARRAY_HPP

