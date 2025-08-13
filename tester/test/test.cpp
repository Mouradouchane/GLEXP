#pragma once 

#ifndef TEST_CPP
#define TEST_CPP

#include "test.hpp"
#include "time/time.hpp"

/*
	class test constructor
*/

u64 test::id_counter = 1;
test::test(std::string const& test_name, bool (*test_function)() ){

	this->id   = test::id_counter;
	this->name = test_name;
	this->ptr_test_function = test_function;

	test::id_counter += 1;
}

/*
	class test public function's
*/

u64 test::get_id() const {
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


#endif