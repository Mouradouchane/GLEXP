#pragma once 

#ifndef o_stringS_HPP
#define o_stringS_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/memory/memory_heap.hpp"

/*
	note: currently we using std::string under wrapper !
		  but latter we could move to core::string or core::c_string .
*/
#include <string>
using string = std::string;

namespace core {

struct c_string {
	core::memory_allocator* allocator_ = nullptr;

	char* start   = nullptr;
	char* end     = nullptr;
	u32   count   = NULL;
};

DLL_API_CLASS o_string {

private	:
	core::memory_allocator* allocator_ = nullptr;

	char* start_  = nullptr;
	u32   size_   = 0;
	u32   count_  = 0;

public : 
	// constructor's
	o_string() = default;
	o_string(const char* _string , core::memory_allocator* memory_allocator = nullptr);
	o_string(const char* _string , u32 count , core::memory_allocator* memory_allocator = nullptr);
	o_string(u32 o_string_size , core::memory_allocator* memory_allocator = nullptr);

	// copy 
	o_string(o_string const& copy_string , core::memory_allocator* memory_allocator = nullptr);
	// move 
	o_string(o_string&& move_string);

	// destructor
	~o_string();
	
	// operator's
	char*     operator[] (u32 index);
	o_string& operator=  (core::o_string const& copy_string);
	o_string& operator=  (const char* copy_string);
	bool      operator== (core::o_string const& other_string);
	bool      operator+= (const char* other_string);

	// public function's
	u32 length(o_string& _string);
	u32 size(o_string& _string);

	const char* c_str(o_string const& _string);
	const char* begin(o_string const& _string);
	const char*   end(o_string const& _string);

	// static function's
	static bool copy(o_string const& source, o_string& destination);
	static bool move(o_string& source, o_string& destination);

}; // class core::string end

} // namespace core end

#endif