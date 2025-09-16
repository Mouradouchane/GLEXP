#pragma once 

#ifndef TEST_HPP
#define TEST_HPP

#include <string>
#include <vector>
#include <initializer_list>
#include "core/types.hpp"

struct test_result {
	u64  id        = NULL;
	bool success   = false;
	u64  last_exec_time = 0u;
};

struct old_test_result {
	bool success     = false;
	u64  exec_time   = 0u;
};

// test class to represent test with its variables as object 
class test {

private: 
	static u64 id_counter;

private: // test private variables
	
	u64  id = NULL;
	u64  last_exec_time   = 0u; // nano second
	bool last_exec_result = false;
	bool (*ptr_test_function)() = nullptr;
	std::string name;

public :

	// constructor / destructor
	test() = delete;
	test(std::string const& test_name , bool (*test_function)() );
	~test() = default;

	// test public functions
	u64 get_id() const;
	std::string get_test_name() const;
	test_result get_test_result();
	test_result get_test_result() const;
	test_result run_test( );

}; // class test end

#endif // TEST_HPP