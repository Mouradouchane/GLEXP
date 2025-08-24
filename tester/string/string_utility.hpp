#pragma once 

#ifndef STRING_UTILITY_HPP
#define STRING_UTILITY_HPP

#include <vector>
#include <string>
#include "common/types.hpp"

std::vector<std::string>* split_string_by_space(std::string const& str);

std::vector<u64>* strings_to_u64_ids(std::string* start, std::string* end);

#endif