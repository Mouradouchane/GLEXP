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
#include "application.hpp"

namespace tester {

	bool make_log_file = true;
	bool running = true;

	std::map<u64, group> groups;
	std::map<u64, test>  tests;

	bool proto_test_success() {
		bool x = false;
		u32  y = x + 2;
		u64  z = y * x;
		for (u32 i = 1; i < 150; i++) z = i * x * y;
		return true;
	}
	bool proto_test_fail() {
		int y = 1 + 2;
		bool x = false;
		u64 z = y * x;
		for (u32 i = 0; i < 100; i++) z = i * x * y;
		return false;
	}

	void add_group(group& _group) {
		tester::groups.insert({ _group.get_id() , _group });
	}

	void add_test(test& _test) {
		tester::tests.insert({ _test.get_id() , _test });
	}

	void init() { // setup tests/groups

		tester::add_group(
			group("stack_tests", {
				test("test_push_1", proto_test_fail),
				test("test_push_2", proto_test_success),
				test("test_pop"   , proto_test_fail),
				test("test_peek_0", proto_test_success),
				test("test_per2"  , proto_test_fail),
			})
		);

		tester::add_group(
			group("g_tests", {
				test("tesg_push_1", proto_test_success),
				test("tesg_peek_0", proto_test_fail),
				test("tesg_per2"  , proto_test_success),
			})
		);

		tester::add_group(
			group("k_tests", {
				test("kh_1", proto_test_fail),
				test("kh_2", proto_test_fail),
				test("k"   , proto_test_fail),
				test("kk_0", proto_test_fail),
			})
		);

		tester::add_test(test("uq_1", proto_test_fail));
		tester::add_test(test("uq_2", proto_test_success));
		tester::add_test(test("uq_3", proto_test_success));
		tester::add_test(test("uq_4", proto_test_success));
		tester::add_test(test("uq_5", proto_test_success));
		tester::add_test(test("uq_6", proto_test_fail));

		logger::print_help();
	}
	
	std::vector<std::string>* split_string_by_space(std::string const& str) {

		std::vector<std::string>* tokens = new std::vector<std::string>();
		std::istringstream iss(str);
		std::string token;

		while (iss >> token) { // Extract words until the end of the stream
			tokens->push_back(token);
		}
		return tokens;
	}

	std::vector<u64>* strings_to_u64_ids(std::string* start, std::string* end) {

		std::vector<u64>* ids = new std::vector<u64>( u64((end+1) - start) );
		
		u64 i = 0;
		for (std::string* str = start ; str != (end+1); str++) {

			const char* s = str->c_str();
			char*       e = ((char*)s) + str->size();

			*(ids->begin() + i) = u64( std::strtoul( s, &e , 10) );
			i++;
		}

		return ids;
	}

	void execute(std::string const& user_command) {

		// get command and its args
		std::vector<std::string>* command_toknes = split_string_by_space(user_command);

		if (command_toknes->size() == 0) return;

		std::string command = *command_toknes->begin();

		// find command to execute
		if (command == "help") {
			logger::print_help();
			return;
		}

		if (command == "list") {
			logger::print_groups(groups);
			logger::print_tests(tests);
			return;
		}

		if (command == "exit") {
			running = false;
			return;
		}

		if (command == "save") {
			// todo : implement save system

			return;
		}

		if (command == "exec_test") {

			if (command_toknes->size() <= 1) {
				logger::error("no argument found with command exec_test");
				logger::hint("exec_test id1 id2 ....");
				logger::hint("exec_test *");

				return;
			}

			if ((*command_toknes)[1] == "*") {
				
				logger::hint("Start All The Ungrouped Tests ...");
				for (auto& pair : tests) {
					pair.second.run_test();
				}
				logger::hint("Ungrouped Tests Finished !");

				logger::print_tests_results(tests);
				
				return;
			}

			std::vector<u64>* ids = strings_to_u64_ids(
				&*(command_toknes->begin() + 1), 
				&*(command_toknes->rbegin())
			);

			for (u64 id : *ids) {
				
				auto pair = tests.find(id);

				if (pair == tests.end()) {
					logger::error("Test with ID " + std::to_string(id) + " Not Found !");
				}
				else {
					test& _test = pair->second;

					logger::hint("Test " + _test.get_test_name() + " with ID:" + std::to_string(_test.get_id()) + " Found !");
					_test.run_test();

					logger::print_test_result(_test);
				}
				
			}

			delete ids;
			return;
		}

		if (command == "exec_group") {

			if (command_toknes->size() <= 1) {
				logger::error("no argument found with command exec_group");
				logger::hint("exec_group id1 id2 ....");
				logger::hint("exec_group *");

				return;
			}

			if ((*command_toknes)[1] == "*") {

				for (auto& pair : groups) {
					logger::hint("Group " + pair.second.get_name() + " with ID:" + std::to_string(pair.second.get_id()) + " Found !");
					pair.second.run_all_tests();
					logger::hint("Group " + pair.second.get_name() + " Finished in " + std::to_string(pair.second.get_exec_time()) + "ns !");

					logger::print_group_results(pair.second);
				}

				return;
			}

			std::vector<u64>* ids = strings_to_u64_ids(
				&*(command_toknes->begin() + 1), 
				&*(command_toknes->rbegin())
			);

			for (u64 id : *ids) {
				auto pair = groups.find(id);

				if (pair == groups.end()) {
					logger::error("Group with ID " + std::to_string(id) + " Not Found !");
				}
				else {
					logger::hint("Group " + pair->second.get_name() + " with ID:" + std::to_string(id) + " Found !");
					pair->second.run_all_tests();
					logger::hint("Group " + pair->second.get_name() + " Finished in " + std::to_string(pair->second.get_exec_time()) + "ns !");

					logger::print_group_results(pair->second);
				}
			}

			delete ids;
			return;
		}

		if (command == "exec_all") {

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
		// ask for : save test result into .log file
		
	}

}

#endif // APPLICATION_CPP
