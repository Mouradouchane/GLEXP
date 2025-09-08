#pragma once 

#ifndef STRING_UTILITY_HPP
#define STRING_UTILITY_HPP

#include <vector>
#include <string>
#include "common/types.hpp"
#include "tools/tester/test/test.hpp"

std::string test_to_string(test const& _test);

bool ends_with(const std::string& str, const std::string& ending);

std::vector<std::string>* split_string_by_space(std::string const& str);

u64 string_to_u64(std::string const& str_value);
std::vector<u64>* strings_to_u64_ids(std::string* start, std::string* end);

#endif