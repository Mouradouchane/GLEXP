#pragma once 

#ifndef TEST_CPP
#define TEST_CPP

#include <chrono>
#include "test.hpp"

#define COUNT_TEST_EXEC_TIME(FUNCTION , PTR_TEST) {\
				auto start_time = std::chrono::high_resolution_clock::now();\
				PTR_TEST->last_exec_result = FUNCTION();\
				auto end_time = std::chrono::high_resolution_clock::now();\
				\
				auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);\
				PTR_TEST->last_exec_time = u64(duration.count());\
		};

#define COUNT_GROUP_EXEC_TIME(PTR_GROUP) {\
				auto start_time = std::chrono::high_resolution_clock::now();\
				for (test& _test : PTR_GROUP->tests) {\
					_test.run_test(true);\
				}\
				auto end_time = std::chrono::high_resolution_clock::now();\
				\
				auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);\
				PTR_GROUP->last_exec_time = u64(duration.count());\
		};



/*
	class test constructor
*/
test::test(u32 id, std::string const& test_name, bool (*test_function)() ){

	this->id = id;
	this->name = test_name;
	this->ptr_test_function = test_function;

}

/*
	class test public function's
*/

u32 test::get_id() const {
	return this->id;
}

std::string test::get_test_name() const {
	return this->name;
}

test_result test::get_test_result() const {
	return test_result{
		this->id,
		this->last_exec_result,
		this->last_exec_time
	};
}

test_result test::run_test(bool count_test_execution_time) {

	if (count_test_execution_time) {
		COUNT_TEST_EXEC_TIME(this->ptr_test_function, this);
	}
	else {
		this->last_exec_result = this->ptr_test_function();
	}

	return test_result{
		this->id,
		this->last_exec_result,
		this->last_exec_time
	};
}


/*
	class gourp constructor
*/
group::group(u32 group_id, std::string const& group_name, std::initializer_list<test> const& testes) {

	this->id    = group_id;
	this->name  = group_name;
	this->tests = testes;

}

/*
	class group public function's
*/

u32 group::get_id() const {
	return this->id;
}

std::string group::get_name() const {
	return this->name;
}

std::vector<test>& group::get_tests_list() {
	return this->tests;
}

void group::run_all_testes(bool count_group_execution_time) {
	
	// todo: add time exection counting
	if (count_group_execution_time) {
		COUNT_GROUP_EXEC_TIME(this);
	}
	else {
		this->last_exec_time = 0u;
		for (test& _test : this->tests) {
			_test.run_test();
		}
	}

}

#endif