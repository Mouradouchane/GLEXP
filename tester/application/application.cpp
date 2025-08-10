#pragma once 

#ifndef APPLICATION_CPP
#define APPLICATION_CPP

#include <iostream>
#include "log/log.hpp"
#include "test/test.hpp"
#include "application.hpp"

namespace tester {

	bool make_log_file = true;
	bool running = true;

	bool proto_test() {
		return true;
	}
	bool proto_test1() {
		return false;
	}

	void init() {
		// setup tests

		group stack_tests(1, "stack_tests", {
			test(1,"test_push_1", proto_test),
			test(3,"test_push_2", proto_test),
			test(2,"test_pop", proto_test),
			test(4,"test_peek_0", proto_test),
			test(8,"test_per2", proto_test),
		});

		logger::print_group(stack_tests);

		test uqs[] = {
			test(1, "uq_test_1", proto_test),
			test(2, "uq_test_2", proto_test1),
			test(3, "uq_test3", proto_test),
			test(4, "uq_test_4", proto_test),
			test(5, "uq_test5", proto_test1),
		};

		logger::print_tests(uqs, 5);

		for (u32 i = 0; i < 5; i++) uqs[i].run_test(true);
		logger::print_tests_results(uqs , 5);

		std::string command = logger::get_user_input();
	}
	
	void run() {
	
		while (running) {
			// list tests

			// take input

			// run tests

			// show tests result

			// take input
		}

	}

	void shutdown() {
		// ask for : save test result into .log file
		
	}

}

#endif // APPLICATION_CPP
