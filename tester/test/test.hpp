#pragma once 

#ifndef TEST_HPP
#define TEST_HPP

#include <string>
#include <vector>
#include <initializer_list>

typedef double   f64;
typedef uint32_t u32;
typedef uint64_t u64;

struct test_result {
	u32  id        = NULL;
	bool success   = false;
	u64  last_exec_time = 0u;
};

// test class to represent test with its variables as object 
class test {

private: // test private variables
	
	u32  id = 0;
	u64  last_exec_time   = 0u; // nano second
	bool last_exec_result = false;
	bool (*ptr_test_function)() = nullptr;
	std::string name;

public :

	// constructor / destructor
	 test(u32 id, std::string const& test_name , bool (*test_function)() );
	~test() = default;

	// test public functions
	u32 get_id() const;
	std::string get_test_name() const;
	test_result get_test_result() const;
	test_result run_test(bool count_test_execution_time = false);

}; // class test end


class group { // this class --> just to group a bunch of tests together

private: // group private variables
	u32 id = 0;
	u64 last_exec_time = 0u;
	std::string name;
	std::vector<test> tests;

public:
	// constructor / destructor
	 group(u32 group_id, std::string const& group_name, std::initializer_list<test> const& testes);
	~group() = default;

	// group public functions
	u32 get_id() const;
	std::string get_name() const;
	std::vector<test>& get_tests_list();

	void run_all_testes(bool count_group_execution_time = false);

}; // class group end

#endif // TEST_HPP