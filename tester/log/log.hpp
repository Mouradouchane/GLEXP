#pragma once 

#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include "test/test.hpp"

namespace logger {

	std::string get_user_input();

	void print_test(test const& _test);
	void print_tests(test* _tests , u32 size);
	void print_group(group& _group);

	void print_test_result(test const& _test);
	void print_tests_results(test* _test , u32 size);
	void print_group_results(group const& _group);

	// this function used by init::shutdown() to save results into .log file
	bool save_result_into_log_file();

}


#endif // !LOG_HPP