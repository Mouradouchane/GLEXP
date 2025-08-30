#pragma once 

#ifndef GROUP_CPP
#define GROUP_CPP

#include <algorithm>
#include "group.hpp"
#include "time/time.hpp"

bool compare_tests(test const& a, test const& b) noexcept {
	return a.get_id() < b.get_id();
}

// class group static variables 
u64 group::id_counter = 1;

/*
	class gourp constructor
*/

group::group(std::string const& group_name, std::initializer_list<test> const& testes) {

	this->id    = group::id_counter;
	this->name  = group_name;
	this->tests = testes;

	// sort tests by id
	std::sort(this->tests.begin(), this->tests.end(), compare_tests);

	group::id_counter += 1;
}

/*
	class group public function's
*/

u64 group::get_id() const {
	return this->id;
}

u64 group::get_exec_time() {
	return this->last_exec_time;
}
u64 group::get_exec_time() const {
	return this->last_exec_time;
}

std::string group::get_name(){
	return std::string(this->name);
}

std::string group::get_name() const {
	return std::string(this->name);
}

std::vector<test> const& group::get_tests_list() {
	return this->tests;
}

std::vector<test> const& group::get_tests_list() const {
	return this->tests;
}

void group::run_few_tests(std::initializer_list<u64> ids) {

	// i know its bad but its just for calling few tests :) 

	time_point start_time = std::chrono::high_resolution_clock::now();
	for (u64 id : ids) {
		for (test& _test : this->tests) {
			if (_test.get_id() == id) _test.run_test();
		}
	}
	time_point  end_time = std::chrono::high_resolution_clock::now();
	nano_second duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

	this->last_exec_time = u64(duration.count());
}

void group::run_all_tests( ) {

	time_point start_time = std::chrono::steady_clock::now();

	for (test& _test : this->tests) {
		_test.run_test();
	}

	time_point  end_time  = std::chrono::steady_clock::now();
	nano_second duration  = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		
	this->last_exec_time = u64(duration.count());
}

#endif