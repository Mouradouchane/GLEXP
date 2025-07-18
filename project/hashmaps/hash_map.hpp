#pragma once

#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP

#include <vector>
#include <array>
#include "types.hpp"
#include "memory.hpp"

typedef u32 hash_key;

template<typename _key, typename _val> struct pair {
	_key key;
	_val value;
};

/*
	hasm_map is a chaining hash table for <key , value>
	note: chaining rows are std::vector not linked-list
*/

template<typename key, typename value> class hash_map {

private:
	u16 _size = 64;
	u16 _row  = 16;

	using table = std::vector< 
		pair<key, value>, 
		custom_allocator<pair<key, value>>
	>;

	std::vector<table , custom_allocator<table>> map;

	hash_key (*hash_function)(key const& _key) = nullptr;

public:
	// constructor's / destructor
	hash_map() = default;
	hash_map(
		u32 (*hashing_function)(key const& _key), 
		u16 map_size = 64,
		u16 row_size = 16
	);

	~hash_map();

	// public function's
	value  get_value(key const& _key);
	value* get_value_ptr(key const& _key);

	bool   search(key const& _key);

	bool   set(key const& _key , value const& new_value);
	bool   insert(key const& _key, value const& _value);
	bool   remove(key _key);

};


// todo: get good hash function for uint and string
template<typename key> u32 hash_keys32(u32 const& _key) {

}
template<typename key> u64 hash_keys64(u64 const& _key) {

}

#endif