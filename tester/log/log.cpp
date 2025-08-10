#pragma once 

#ifndef LOG_CPP
#define LOG_CPP

#include "tabulate/table.hpp"
#include <iostream>
#include "log.hpp"

using namespace tabulate;

std::string logger::get_user_input() {

	std::string in_str;
	std::cout << "\n";
	std::cout << "command: "; std::cin >> in_str;

	return in_str;
}

void logger::print_test(test const& _test) {
	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" });

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	table.add_row(RowStream{} << std::to_string(_test.get_id()) << _test.get_test_name() );
	std::cout << table << "\n";
}

void logger::print_tests(test* _tests, u32 size) {
	std::cout << "Ungrouped Tests: " << size << " tests\n";

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" });

	table[0].format().font_color(Color::cyan);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	for (test* _test = _tests; _test != (_tests + size); _test++) {
		table.add_row(RowStream{} << _test->get_id() << _test->get_test_name());
	}

	std::cout << table << "\n";
}

void logger::print_group(group& _group) {

	std::cout << "Group: " << _group.get_name() << ", ID: " << _group.get_id() << ", Include: " << _group.get_tests_list().size() << " tests\n";

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" });

	table[0].format().font_color(Color::cyan);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	for (test& _test : _group.get_tests_list() ) {
		table.add_row(RowStream{} << _test.get_id() << _test.get_test_name());
	}

	std::cout << table << "\n";
}

void logger::print_test_result(test const& _test) {

	test_result result = _test.get_test_result();

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" , "execute-time" , "test-result"});

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	table.add_row(
		RowStream{} << std::to_string(_test.get_id()) << _test.get_test_name() <<
		(result.last_exec_time > 0 ? (std::to_string(result.last_exec_time) + " ns") : "uncounted") <<
		(result.success ? "successed" : "failed")
	);

	table[1].format().font_background_color( result.success ? Color::green : Color::red);	
	std::cout << table << "\n";
}

void logger::print_tests_results(test* _test , u32 size) {

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" , "execute-time" , "test-result"});

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	for (u32 i = 0; i < size; i++) {
		test_result test = _test[i].get_test_result();

		table.add_row(
			RowStream{} << 
			std::to_string(test.id) << _test[i].get_test_name() <<
			(test.last_exec_time > 0 ? (std::to_string(test.last_exec_time) + " ns") : "uncounted") <<
			(test.success ? "successed" : "failed")
		);

		table[i+1].format().font_background_color( test.success ? Color::green : Color::red);	
	}

	std::cout << table << "\n";
}

void logger::print_group_results(group const& _group) {

}

#endif