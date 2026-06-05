#pragma once 

#ifndef CORE_STRING_HPP
#define CORE_STRING_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/memory/dynamic_allocator/dynamic_allocator.hpp"

#include <string>

/*
	note: currently we using std::string under wrapper !
			but latter we could move to core::c_string or core::o_string .
*/
typedef std::string string;

#ifndef STRING
	#define STRING std::string
#endif

namespace core {
	
	struct c_string {
		core::dynamic_allocator* allocator = nullptr;

		char* start   = nullptr;
		char* end     = nullptr;
		u32   count   = NULL;
	};

	DLL_API_CLASS o_string {

	private	:
		core::dynamic_allocator* allocator_ = nullptr;

		char* start_  = nullptr;
		u32   size_   = 0;
		u32   count_  = 0;

	public : 
		// constructor's
		o_string() = default;
		o_string(const char* _string , core::dynamic_allocator* memory_allocator = nullptr);
		o_string(const char* _string , u32 count , core::dynamic_allocator* memory_allocator = nullptr);
		o_string(u32 o_string_size , core::dynamic_allocator* memory_allocator = nullptr);

		// copy 
		o_string(o_string const& copy_string , core::dynamic_allocator* memory_allocator = nullptr);
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

	}; 
	// class o_string end

	DLL_API STRING pointer_to_hex_string(void* pointer);

} // namespace core end

#endif