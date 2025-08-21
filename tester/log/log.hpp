#pragma once 

#ifndef LOG_HPP
#define LOG_HPP

#include <map>
#include <string>
#include "group/group.hpp"
#include "test/test.hpp"

namespace logger {

	std::string get_user_input();

	void hint(std::string  const& hint_message);
	void warn(std::string  const& warn_message);
	void error(std::string const& error_message);

	void print_help();

	void print_test(test const& _test);
	void print_tests(std::map<u64 , test> const& tests);

	void print_group(group const& _group);
	void print_groups(std::map<u64 , group> const& groups);

	void print_test_result(test const& _test);
	void print_tests_results(test* _test , u32 size);
	void print_tests_results(std::map<u64, test>& tests);
	void print_group_results(group const& _group);

	// this function used by init::shutdown() to save results into .log file
	bool save_result_into_log_file();

}


#endif // !LOG_HPP