#pragma once 

#ifndef STRINGS_HPP
#define STRINGS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/memory/memory_heap.hpp"

namespace core {

DLL_API struct c_string {
	char* start  = nullptr;
	char* end    = nullptr;
	u32   count   = NULL;
};

DLL_API class string {

private	:
	char* start = nullptr;
	char* end   = nullptr;
	u32   count  = NULL;

public : 
	// constructor's
	string() = default;
	string(const char* _string);
	string(const char* _string , u32 count);
	string(u32 string_size);

	// destructor
	~string();

	// operator's
	char*   operator[] (u32 index);
	string& operator= (const string& copy_string);

	// static function's
	static u32 count(string& str);
	static u32 length(string& str);

	static bool copy(string& destination, const string& source);
	static bool move(string& destination, string& source);

	static const char* c_str(string const& _string);
	static const char* begin(string const& _string);
	static const char*   end(string const& _string);

	static bool clear(string const& _string);
	static bool fill(string const& _string, char _value);

	// note: this mean global_memory reallocation which is means old "start/end" pointers gonna be invalid !!!
	static bool resize(u32 new_size);

	static bool compare(string const& a, string const& b);

}; // class string

} // namespace core

#endif