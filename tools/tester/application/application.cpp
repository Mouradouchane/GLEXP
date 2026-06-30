#pragma once 

#ifndef TESTER_APPLICATION_CPP
#define TESTER_APPLICATION_CPP

#define UNIT_TEST

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <map>

#include "tools/tester/test/test.hpp"
#include "tools/tester/group/group.hpp"
#include "tools/tester/log/log.hpp"
#include "tools/tester/string/string_utility.hpp"

// todo: add tests for these asap
// #include "core/events/keyboard/keyboard.hpp"
// #include "core/references/references.hpp"
// #include "core/window/window.hpp"
#include "core/logger/logger.hpp"

// enable logger
static auto _tester_app_cpp_logger_ = CORE_GET_LOGGER(UNIT_TESTER_LOGGER);
#define _LOGGER_ _tester_app_cpp_logger_

/*
	include unit-test's function's
	- todo: add memory allocator for the arrays tests
*/ 
// #include "tools/tester/unit_tests/arrays/array_tests.hpp"
// #include "tools/tester/unit_tests/arrays/dynamic_array_tests.hpp"
#include "tools/tester/unit_tests/memory_tests/dynamic_allocator_tests/behavior_on_st.hpp"

#include "application.hpp"

#define TESTER_ADD_TEST(FUNCTION)  test(#FUNCTION, FUNCTION)

// few private functions for tester::execute() function
static inline void exec_all_function();
static inline void exec_tests_function( std::string& command, std::vector<std::string>* command_toknes);
static inline void exec_groups_function(std::string& command, std::vector<std::string>* command_toknes);

namespace tester {

	bool running = true;

	// used to save all results in one string to be saved in .log file later
	std::string results_history;
	std::map<std::string, old_test_result> old_tests;

	std::map<u64, group> groups;
	std::map<u64, test>  tests;

	void add_group(group const& _group) {
		tester::groups.insert({ _group.get_id() , _group });
	}

	void add_test(test const& _test) {
		tester::tests.insert({ _test.get_id() , _test });
	}

	void init() { 

		core::logger::init("tester_logger" , core::logger::verbosity_level::trace);
		


		/*
			note: add your unit-tests here
		*/ 
		/*
		add_group(
			group("core::array tests", {
				TESTER_ADD_TEST(array_t_construct_count_size_and_begin_end),
				TESTER_ADD_TEST(get_set_and_operator_index),
				TESTER_ADD_TEST(ll),
				TESTER_ADD_TEST(opy_ctor_and_static_copy),
				TESTER_ADD_TEST(rray_t_static_move_move_assign_move_ctor),
				TESTER_ADD_TEST(_trivial_destruction),
				TESTER_ADD_TEST(),
				TESTER_ADD_TEST(e_reallocate),
				TESTER_ADD_TEST(ases),
				TESTER_ADD_TEST(y_t_copy_into_existing_destination),
				TESTER_ADD_TEST(sic_construction),
				TESTER_ADD_TEST(_constructor_from_pointer_trivial),
				TESTER_ADD_TEST(ay_t_array_constructor_from_pointer_nontrivial),
				TESTER_ADD_TEST(y_constructor_trivial),
				TESTER_ADD_TEST(_array_copy_constructor_nontrivial),
				TESTER_ADD_TEST(_t_array_static_copy_nontrivial),
				TESTER_ADD_TEST(function)
			})
		);

		add_group(
			group("core::dynamic_array tests", {
				TESTER_ADD_TEST(dynamic_arr_t_construction_basics),
				TESTER_ADD_TEST(vialdynamic_arr_t_push_and_resize_trivial),
				TESTER_ADD_TEST(trivial_stringdynamic_arr_t_push_and_resize_nontrivial),
				TESTER_ADD_TEST(ynamic_arr_t_pop_basics_trivial),
				TESTER_ADD_TEST(al_stringdynamic_arr_t_pop_basics_nontrivial),
				TESTER_ADD_TEST(trivialdynamic_arr_t_copy_assignment_nontrivial),
				TESTER_ADD_TEST(ynamic_arr_t_empty_pop_handling),
				TESTER_ADD_TEST(e_and_begin_enddynamic_arr_t_construct_count_size_and_begin_end),
				TESTER_ADD_TEST(r_indexdynamic_arr_t_get_set_and_operator_index),
				TESTER_ADD_TEST(edynamic_arr_t_push_and_auto_resize),
				TESTER_ADD_TEST(dynamic_arr_t_nontrivial_push_pop),
				TESTER_ADD_TEST(mic_arr_t_copy_assignment),
				TESTER_ADD_TEST(mic_arr_t_move_assignment)
			})
		);
		*/

		add_group(
			group("core::dynamic_array tests", {
				TESTER_ADD_TEST(dynamic_allocator_simple_usage_on_st_test_1),
				TESTER_ADD_TEST(dynamic_allocator_simple_usage_on_st_test_2),
				TESTER_ADD_TEST(dynamic_allocate_memory_1),
				TESTER_ADD_TEST(dynamic_allocate_memory_2),
			})
		);

		logger::load_old_tests_from_files(old_tests);
		tester::logger::print_help();
	}

	void execute(std::string const& user_command) {

		// get command and its args
		std::vector<std::string>* command_toknes = split_string_by_space(user_command);

		if (command_toknes->size() == 0) return;

		std::string command = *command_toknes->begin();

		/*
			find command to execute
		*/ 

		if (command == "help") {
			tester::logger::print_help();
			return;
		}

		if (command == "clear_console") {
		#if defined(_WIN32) || defined(_WIN64)
			system("cls");
		#else 
			system("clear");
		#endif
			return;
		}

		if (command == "clear_results") {
			results_history = "";
			CORE_WARN(0,"all current tests results removed from history !");
			return;
		}

		if (command == "list_all") {
			tester::logger::print_groups(groups);
			tester::logger::print_tests(tests);
			return;
		}

		if (command == "list_groups") {
			tester::logger::print_groups(groups);
			return;
		}
		if (command == "list_tests") {
			tester::logger::print_tests(tests);
			return;
		}

		if (command == "exit") {
			running = false;
			return;
		}

		if (command == "save") {
			logger::save_tests_results_in_log_file(results_history);
			return;
		}

		if (command == "exec_test") {
			exec_tests_function(command, command_toknes);
			return;
		}

		if (command == "exec_group") {
			exec_groups_function(command, command_toknes);
			return;
		}

		if (command == "exec_all") {
			exec_all_function();
			return;
		}

		delete command_toknes;
	}

	void run() {

		std::string command = "";

		while (running) {
			command = logger::get_user_input();

			execute(command);
		}

	}

	void shutdown() {
		std::cout << "press (Y) to save results before exit:";

		std::string command = ""; std::getline(std::cin ,command);
		std::cout << "\n";

		if(command == "Y" || command == "y") logger::save_tests_results_in_log_file(results_history);
	}

}


/*
	 private functions for tester
*/

static inline void exec_all_function() {

	// execute all ungrouped tests
	for (auto& pair : tester::tests) {
		pair.second.run_test();
	}
	CORE_INFO("Ungrouped Tests Finished !");

	tester::logger::print_tests_results(tester::tests , tester::results_history, tester::old_tests);

	// execute all groups
	for (auto& pair : tester::groups) {
		CORE_INFO("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
		pair.second.run_all_tests();
		CORE_INFO("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

		tester::logger::print_group_results(pair.second, tester::results_history, tester::old_tests);
	}

}

static inline void exec_tests_function(std::string& command , std::vector<std::string>* command_toknes) {

	if (command_toknes->size() <= 1) {
		CORE_ERROR(0,"no argument found with command exec_test");
		CORE_INFO("exec_test id1 id2 ....");
		CORE_INFO("exec_test *");

		return;
	}

	// execute all ungrouped tests
	if ((*command_toknes)[1] == "*") {

		CORE_INFO("Start All The Ungrouped Tests ...");
		for (auto& pair : tester::tests) {
			pair.second.run_test();
		}
		CORE_INFO("Ungrouped Tests Finished !");

		tester::logger::print_tests_results(tester::tests , tester::results_history , tester::old_tests);

		return;
	}

	std::vector<u64>* ids = strings_to_u64_ids(
		&*(command_toknes->begin() + 1), 
		&*(command_toknes->rbegin())
	);

	// execute few ungrouped tests
	for (u64 id : *ids) {

		auto pair = tester::tests.find(id);

		if (pair == tester::tests.end()) {
			CORE_ERROR(0,"Test with ID " + std::to_string(id) + " Not Found !");
		}
		else {
			test& _test = pair->second;

			CORE_INFO("Test " + _test.get_test_name() + " with ID:" + std::to_string(_test.get_id()) + " Found !");
			_test.run_test();

			tester::logger::print_test_result(_test , tester::results_history, tester::old_tests);
		}

	}

	delete ids;
}

static inline void exec_groups_function(std::string& command, std::vector<std::string>* command_toknes) {

	if (command_toknes->size() <= 1) {
		CORE_ERROR(0,"no argument found with command exec_group");
		CORE_INFO("exec_group id1 id2 ....");
		CORE_INFO("exec_group *");

		return;
	}

	// execute all groups
	if ((*command_toknes)[1] == "*") {

		for (auto& pair : tester::groups) {
			CORE_INFO("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
			pair.second.run_all_tests();
			CORE_INFO("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

			tester::logger::print_group_results(pair.second , tester::results_history, tester::old_tests);
		}

		return;
	}

	std::vector<u64>* ids = strings_to_u64_ids(
		&*(command_toknes->begin() + 1), 
		&*(command_toknes->rbegin())
	);

	// execute few groups
	for (u64 id : *ids) {
		auto pair = tester::groups.find(id);

		if (pair == tester::groups.end()) {
			CORE_ERROR(0,"Group with ID " + std::to_string(id) + " Not Found !");
		}
		else {
			CORE_INFO("Group " + pair->second.get_name() + " with ID:" + std::to_string(id) + " Found !");
			pair->second.run_all_tests();
			CORE_INFO("Group " + pair->second.get_name() + " Finished in " + std::to_string(pair->second.get_exec_time()) + "ns !");

			tester::logger::print_group_results(pair->second , tester::results_history, tester::old_tests);
		}
	}

	delete ids;
}


#endif // APPLICATION_CPP
