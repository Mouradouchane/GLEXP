#pragma once 

#ifndef LOG_HPP
#define LOG_HPP

#include <map>
#include <string>
#include "tools/tester/group/group.hpp"
#include "tools/tester/test/test.hpp"

namespace tester {

	namespace logger {

		std::string get_user_input();

		void print_help();

		void print_test(test const& _test);
		void print_tests(std::map<u64, test> const& tests);

		void print_group(group const& _group);
		void print_groups(std::map<u64, group> const& groups);

		void print_test_result(test const& _test, std::string& results_str, std::map<std::string, old_test_result>& old_results);
		void print_tests_results(std::map<u64, test>& tests, std::string& results_str, std::map<std::string, old_test_result>& old_results);
		void print_group_results(group const& _group, std::string& results_str, std::map<std::string, old_test_result>& old_results);

		// used by "save command or application::shutdown()" to save tests results in .log file
		bool save_tests_results_in_log_file(std::string const& results_as_str);
		void load_old_tests_from_files(std::map<std::string, old_test_result>& output_map);
	}

}

#endif // !LOG_HPP