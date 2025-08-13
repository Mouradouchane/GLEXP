#pragma once 

#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>
#include <vector>
#include <initializer_list>

#include "common/types.hpp"
#include "test/test.hpp"
#include "time/time.hpp"

class group { // this class --> just to group a bunch of tests together

private:
	static u64 id_counter;

private: // group private variables
	u64 id = NULL;
	u64 last_exec_time = 0u;
	std::string name;
	std::vector<test> tests;

public:
	// constructor / destructor
	 group(std::string const& group_name, std::initializer_list<test> const& testes);
	~group() = default;

	// group public functions
	u64 get_id() const;
	std::string get_name() const;
	std::vector<test> const& get_tests_list();

	void run_few_tests(std::initializer_list<u64> ids,bool count_group_execution_time = true);
	void run_all_tests(bool count_group_execution_time = true);

}; // class group end

#endif