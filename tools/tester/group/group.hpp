#pragma once 

#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>
#include <vector>
#include <initializer_list>

#include "core/types.hpp"
#include "tools/tester/test/test.hpp"
#include "tools/tester/time/time.hpp"

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

	u64 get_exec_time();
	u64 get_exec_time() const;

	std::string get_name();
	std::string get_name() const;

	std::vector<test> const& get_tests_list();
	std::vector<test> const& get_tests_list() const;

	void run_few_tests(std::initializer_list<u64> ids);
	void run_all_tests();

}; // class group end

#endif