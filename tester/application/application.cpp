#pragma once 

#ifndef APPLICATION_CPP
#define APPLICATION_CPP

#include <iostream>

#include "log/log.hpp"
#include "test/test.hpp"
#include "group/group.hpp"
#include "command/command.hpp"
#include "application.hpp"

namespace tester {

	bool make_log_file = true;
	bool running = true;

	commands command_list;

	bool proto_test() {
		return true;
	}
	bool proto_test1() {
		return false;
	}

	void init() { // setup tests/groups

		group stack_tests("stack_tests", {
			test("test_push_1", proto_test),
			test("test_push_2", proto_test),
			test("test_pop", proto_test),
			test("test_peek_0", proto_test),
			test("test_per2", proto_test),
		});

		logger::print_group(stack_tests);

		test uqs[] = {
			test("uq_test_1", proto_test),
			test("uq_test_2", proto_test1),
			test("uq_test3", proto_test),
			test("uq_test_4", proto_test),
			test("uq_test5", proto_test1),
		};

		logger::print_tests(uqs, 5);

		for (u32 i = 0; i < 5; i++) uqs[i].run_test(true);
		logger::print_tests_results(uqs , 5);

	}
	
	enum class commands_index : u64 {
		help = 0,
		exit,
		save,
		exec_test,
		exec_group,
		exec_all,
	};

	commands_index handle_user_input(std::string const& user_command) {

		// find command function
		if (user_command == "help") {
			return commands_index::help;
		}
		if (user_command == "exit") {
			return commands_index::exit;
		}
		if (user_command == "save") {
			return commands_index::save;
		}
		if (user_command == "exec_test") {
			return commands_index::exec_test;
		}
		if (user_command == "exec_group") {
			return commands_index::exec_group;
		}
		if (user_command == "exec_all") {
			return commands_index::exec_all;
		}

		return commands_index::help;
	}

	void run() {

		std::string    command = "";
		commands_index command_index = commands_index::help;

		while (running) {
			// take input
			command = logger::get_user_input();

			// handle input 
			command_index = handle_user_input(command);
			
			// execute

		}

	}

	void shutdown() {
		// ask for : save test result into .log file
		
	}

}

#endif // APPLICATION_CPP
