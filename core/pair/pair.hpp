#pragma once 

#ifndef CORE_PAIR_HPP
#define CORE_PAIR_HPP

template<typename first_type, typename second_type> struct pair {
	first_type  first;
	second_type second;
};

template<typename type> struct same_pair {
	type first;
	type second;
};

#endif