#pragma once 

#ifndef APPLICATION_CPP
#define APPLICATION_CPP

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

#include "core/status/status.hpp"
#include "core/event_system/event.hpp"


// include tests
#include "tools/tester/unit_tests/arrays/array_tests.hpp"
#include "tools/tester/unit_tests/arrays/dynamic_array_tests.hpp"

#include "application.hpp"

// few private functions for tester::execute() function
static inline void exec_all_function();
static inline void exec_tests_function(std::string& command, std::vector<std::string>* command_toknes);
static inline void exec_groups_function(std::string& command, std::vector<std::string>* command_toknes);

namespace tester {

	bool running = true;

	// used to save all results in one string to be saved in .log file later
	std::string results_history;
	std::map<std::string, old_test_result> old_tests;

	std::map<u64, group> groups;
	std::map<u64, test>  tests;

	void add_group(group& _group) {
		tester::groups.insert({ _group.get_id() , _group });
	}

	void add_test(test& _test) {
		tester::tests.insert({ _test.get_id() , _test });
	}

	void init() { 

		core::logger::init("tester_logger" , core::logger::verbosity_level::trace);
		core::event_system::init();

		/*
			note: add your unit-tests here
		*/ 

	add_group(
		group("core::array tests", {
			test("array_construct_count_size_and_begin_end",	array_t_construct_count_size_and_begin_end),
			test("array_get_set_and_operator_index",			array_t_get_set_and_operator_index),
			test("array_clear_and_fill",						array_t_clear_and_fill),
			test("array_copy_ctor_and_static_copy",				array_t_copy_ctor_and_static_copy),
			test("array_static_move_move_assign_move_ctor",		array_t_static_move_move_assign_move_ctor),
			test("array_non_trivial_destruction",				array_t_non_trivial_destruction),
			test("array_sort_function",							array_t_sort_function),
			test("array_allocate_reallocate",					array_t_allocate_reallocate),
			test("array_fill_edge_cases",						array_t_fill_edge_cases),
			test("array_copy_into_existing_destination",		array_t_copy_into_existing_destination),
			test("array_basic_construction",					array_t_array_basic_construction),
			test("array_ctor_from_ptr_trivial",					array_t_array_constructor_from_pointer_trivial),
			test("array_ctor_from_ptr_nontrivial_string",		array_t_array_constructor_from_pointer_nontrivial),
			test("array_copy_ctor_trivial",						array_t_array_copy_constructor_trivial),
			test("array_copy_ctor_nontrivial_string",			array_t_array_copy_constructor_nontrivial),
			test("array_static_copy_nontrivial_string",			array_t_array_static_copy_nontrivial),
			test("array_clear_function",						array_t_array_clear_function)
		})
	);

	add_group(
		group("core::dynamic_array tests", {
			test("dynamic_array_construction_basics",				 dynamic_arr_t_construction_basics),
			test("dynamic_array_push_and_resize_trivial",			 dynamic_arr_t_push_and_resize_trivial),
			test("dynamic_array_push_and_resize_nontrivial_string",  dynamic_arr_t_push_and_resize_nontrivial),
			test("dynamic_array_pop_basics_trivial",				 dynamic_arr_t_pop_basics_trivial),
			test("dynamic_array_pop_basics_nontrivial_string",		 dynamic_arr_t_pop_basics_nontrivial),
			test("dynamic_array_copy_assignment_nontrivial",		 dynamic_arr_t_copy_assignment_nontrivial),
			test("dynamic_array_empty_pop_handling",				 dynamic_arr_t_empty_pop_handling),
			test("dynamic_array_construct_count_size_and_begin_end", dynamic_arr_t_construct_count_size_and_begin_end),
			test("dynamic_array_get_set_and_operator_index",		 dynamic_arr_t_get_set_and_operator_index),
			test("dynamic_array_push_and_auto_resize",				 dynamic_arr_t_push_and_auto_resize),
			test("dynamic_array_nontrivial_push_pop",				 dynamic_arr_t_nontrivial_push_pop),
			test("dynamic_array_copy_assignment",					 dynamic_arr_t_copy_assignment),
			test("dynamic_array_move_assignment",					 dynamic_arr_t_move_assignment)
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
			core::logger::warn("all current tests results removed from history !");
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
	core::logger::info("Ungrouped Tests Finished !");

	tester::logger::print_tests_results(tester::tests , tester::results_history, tester::old_tests);

	// execute all groups
	for (auto& pair : tester::groups) {
		core::logger::info("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
		pair.second.run_all_tests();
		core::logger::info("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

		tester::logger::print_group_results(pair.second, tester::results_history, tester::old_tests);
	}

}

static inline void exec_tests_function(std::string& command , std::vector<std::string>* command_toknes) {

	if (command_toknes->size() <= 1) {
		core::logger::error("no argument found with command exec_test");
		core::logger::info("exec_test id1 id2 ....");
		core::logger::info("exec_test *");

		return;
	}

	// execute all ungrouped tests
	if ((*command_toknes)[1] == "*") {

		core::logger::info("Start All The Ungrouped Tests ...");
		for (auto& pair : tester::tests) {
			pair.second.run_test();
		}
		core::logger::info("Ungrouped Tests Finished !");

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
			core::logger::error("Test with ID " + std::to_string(id) + " Not Found !");
		}
		else {
			test& _test = pair->second;

			core::logger::info("Test " + _test.get_test_name() + " with ID:" + std::to_string(_test.get_id()) + " Found !");
			_test.run_test();

			tester::logger::print_test_result(_test , tester::results_history, tester::old_tests);
		}

	}

	delete ids;
}

static inline void exec_groups_function(std::string& command, std::vector<std::string>* command_toknes) {

	if (command_toknes->size() <= 1) {
		core::logger::error("no argument found with command exec_group");
		core::logger::info("exec_group id1 id2 ....");
		core::logger::info("exec_group *");

		return;
	}

	// execute all groups
	if ((*command_toknes)[1] == "*") {

		for (auto& pair : tester::groups) {
			core::logger::info("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
			pair.second.run_all_tests();
			core::logger::info("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

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
			core::logger::error("Group with ID " + std::to_string(id) + " Not Found !");
		}
		else {
			core::logger::info("Group " + pair->second.get_name() + " with ID:" + std::to_string(id) + " Found !");
			pair->second.run_all_tests();
			core::logger::info("Group " + pair->second.get_name() + " Finished in " + std::to_string(pair->second.get_exec_time()) + "ns !");

			tester::logger::print_group_results(pair->second , tester::results_history, tester::old_tests);
		}
	}

	delete ids;
}


#endif // APPLICATION_CPP
