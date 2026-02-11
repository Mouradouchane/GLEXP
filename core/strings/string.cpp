#pragma once 

#ifndef CORE_STRING_CPP
#define CORE_STRING_CPP

#include <sstream>
#include "string.hpp"

namespace core {

	DLL_API string pointer_to_hex_string(void* pointer){
		std::stringstream s_stream;
		s_stream << std::hex << pointer;

		return string( "0x" + s_stream.str());
	}

}

#endif