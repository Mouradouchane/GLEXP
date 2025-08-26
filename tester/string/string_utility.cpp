#pragma once 

#ifndef STRING_UTILITY_CPP
#define STRING_UTILITY_CPP

#include <sstream>
#include "string_utility.hpp"

std::string test_to_string(test const& _test) {
	test_result trslt = _test.get_test_result();
	std::string str_line = "";

	str_line += _test.get_test_name();
	for (size_t i = str_line.size(); i < 32; i++) {
		str_line += " ";
	}
	std::string time_str = std::to_string(trslt.last_exec_time) + "ns ";
	str_line += time_str;
	for (size_t i = time_str.size(); i < 16; i++) {
		str_line += " ";
	}

	str_line += (trslt.success ? "successed" : "failed");
	str_line += "\n";

	return str_line;
}

bool ends_with(const std::string& str, const std::string& ending) {

	if (str.length() >= ending.length()) {
		return (str.compare(str.length() - ending.length(), ending.length(), ending) == 0);
	}
	
	return false;
}

std::vector<std::string>* split_string_by_space(std::string const& str) {

	std::vector<std::string>* tokens = new std::vector<std::string>();
	std::istringstream iss(str);
	std::string token;

	while (iss >> token) { // Extract words until the end of the stream
		tokens->push_back(token);
	}

	return tokens;
}

u64 string_to_u64(std::string const& str_value) {
	const char* s = str_value.c_str();
	char*       e = ((char*)s) + str_value.size();
	return u64( std::strtoul( s, &e , 10) );
}

std::vector<u64>* strings_to_u64_ids(std::string* start, std::string* end) {

	std::vector<u64>* ids = new std::vector<u64>( u64((end+1) - start) );

	u64 i = 0;
	for (std::string* str = start ; str != (end+1); str++) {
		*(ids->begin() + i) = string_to_u64(*start);
		i++;
	}

	return ids;
}

#endif