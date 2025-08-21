#pragma once 

#ifndef TEST_CPP
#define TEST_CPP

#include "test.hpp"
#include "time/time.hpp"
#include "log/log.hpp"

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

test_result test::get_test_result() {
	return test_result{
		this->id,
		this->last_exec_result,
		this->last_exec_time
	};
}

test_result test::get_test_result() const {
	return test_result{
		this->id,
		this->last_exec_result,
		this->last_exec_time
	};
}

test_result test::run_test( ) {

	time_point start_time  = std::chrono::steady_clock::now();
	this->last_exec_result = this->ptr_test_function();
	time_point  end_time   = std::chrono::steady_clock::now();

	nano_second duration   = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
	
	this->last_exec_time   = u64(duration.count());

	if (this->last_exec_time == 0) {
		logger::warn("0ns time-execution detected in test: " + this->name + " ID: " + std::to_string(this->id));
		logger::warn("reasons :\ntest function could be optimitzed by the compiler !\ntest function could be way faster than steady_clock !");
	}

	return test_result{
		this->id,
		this->last_exec_result,
		this->last_exec_time
	};
}


#endif