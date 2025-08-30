#pragma once 

#ifndef APPLICATION_CPP
#define APPLICATION_CPP

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <map>

#include "test/test.hpp"
#include "group/group.hpp"
#include "log/log.hpp"
#include "string/string_utility.hpp"
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

	// setup tests and groups + loading old results from files
	void init() { 

	


		logger::load_old_tests_from_files(old_tests);
		logger::print_help();
	}

	void execute(std::string const& user_command) {

		// get command and its args
		std::vector<std::string>* command_toknes = split_string_by_space(user_command);

		if (command_toknes->size() == 0) return;

		std::string command = *command_toknes->begin();

		/*/
			find command to execute
		*/ 

		if (command == "help") {
			logger::print_help();
			return;
		}

		if (command == "clear_console") {
		#ifdef _WIN32 || _WIN64
			system("cls");
		#else 
			system("clear");
		#endif
			return;
		}

		if (command == "clear_results") {
			results_history = "";
			logger::warn("all current tests results removed from history !");
			return;
		}

		if (command == "list_all") {
			logger::print_groups(groups);
			logger::print_tests(tests);
			return;
		}

		if (command == "list_groups") {
			logger::print_groups(groups);
			return;
		}
		if (command == "list_tests") {
			logger::print_tests(tests);
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
	logger::hint("Ungrouped Tests Finished !");

	logger::print_tests_results(tester::tests , tester::results_history, tester::old_tests);

	// execute all groups
	for (auto& pair : tester::groups) {
		logger::hint("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
		pair.second.run_all_tests();
		logger::hint("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

		logger::print_group_results(pair.second, tester::results_history, tester::old_tests);
	}

}

static inline void exec_tests_function(std::string& command , std::vector<std::string>* command_toknes) {

	if (command_toknes->size() <= 1) {
		logger::error("no argument found with command exec_test");
		logger::hint("exec_test id1 id2 ....");
		logger::hint("exec_test *");

		return;
	}

	// execute all ungrouped tests
	if ((*command_toknes)[1] == "*") {

		logger::hint("Start All The Ungrouped Tests ...");
		for (auto& pair : tester::tests) {
			pair.second.run_test();
		}
		logger::hint("Ungrouped Tests Finished !");

		logger::print_tests_results(tester::tests , tester::results_history , tester::old_tests);

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
			logger::error("Test with ID " + std::to_string(id) + " Not Found !");
		}
		else {
			test& _test = pair->second;

			logger::hint("Test " + _test.get_test_name() + " with ID:" + std::to_string(_test.get_id()) + " Found !");
			_test.run_test();

			logger::print_test_result(_test , tester::results_history, tester::old_tests);
		}

	}

	delete ids;
}

static inline void exec_groups_function(std::string& command, std::vector<std::string>* command_toknes) {

	if (command_toknes->size() <= 1) {
		logger::error("no argument found with command exec_group");
		logger::hint("exec_group id1 id2 ....");
		logger::hint("exec_group *");

		return;
	}

	// execute all groups
	if ((*command_toknes)[1] == "*") {

		for (auto& pair : tester::groups) {
			logger::hint("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
			pair.second.run_all_tests();
			logger::hint("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

			logger::print_group_results(pair.second , tester::results_history, tester::old_tests);
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
			logger::error("Group with ID " + std::to_string(id) + " Not Found !");
		}
		else {
			logger::hint("Group " + pair->second.get_name() + " with ID:" + std::to_string(id) + " Found !");
			pair->second.run_all_tests();
			logger::hint("Group " + pair->second.get_name() + " Finished in " + std::to_string(pair->second.get_exec_time()) + "ns !");

			logger::print_group_results(pair->second , tester::results_history, tester::old_tests);
		}
	}

	delete ids;
}


#endif // APPLICATION_CPP
