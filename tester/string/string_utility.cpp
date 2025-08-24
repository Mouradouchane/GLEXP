#pragma once 

#ifndef STRING_UTILITY_CPP
#define STRING_UTILITY_CPP

#include <sstream>
#include "string_utility.hpp"

std::vector<std::string>* split_string_by_space(std::string const& str) {

	std::vector<std::string>* tokens = new std::vector<std::string>();
	std::istringstream iss(str);
	std::string token;

	while (iss >> token) { // Extract words until the end of the stream
		tokens->push_back(token);
	}
	return tokens;
}

std::vector<u64>* strings_to_u64_ids(std::string* start, std::string* end) {

	std::vector<u64>* ids = new std::vector<u64>( u64((end+1) - start) );

	u64 i = 0;
	for (std::string* str = start ; str != (end+1); str++) {

		const char* s = str->c_str();
		char*       e = ((char*)s) + str->size();

		*(ids->begin() + i) = u64( std::strtoul( s, &e , 10) );
		i++;
	}

	return ids;
}

#endif