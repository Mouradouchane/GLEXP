#pragma once

#ifndef HASH_MAP_CPP
#define HASH_MAP_CPP

#include "assert.hpp"
#include "hash_map.hpp"

/*
	 constructor
*/
template<typename key, typename value> hash_map<key,value>::hash_map(
	u32 (*hashing_function)(key const& _key), 
	u16 map_size = 64,
	u16 row_size = 16
) {
	CRASH_IF((map_size == 0) || (row_size == 0) , "hash_map: 0 size hash_map not allowed !")

	this->hash_function = hashing_function;
	this->_size = map_size;
	this->_row = row_size;

	this->map = std::vector<table, custom_allocator<table>>(this->_size);

	for (table& _table : this->map) {
		_table = table(this->_row);
	}

}

// todo: implement desturctor
template<typename key, typename value> hash_map<key, value>::~hash_map() {

}

/*
	hash_map function's
*/

// todo: implement insert
template<typename key, typename value> bool hash_map<key,value>::insert(
	key const& _key , value const& _value
) {
	hash_key hash = this->hash_function(_key);
	ASSERT_EXP(hash >= this->_size, "hash::insert : hash function result out of range !");

	// check for duplication --> no allowed 
	if (this->search(_key)) return false;


	return true;
}

#endif