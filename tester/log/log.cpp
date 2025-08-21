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
	std::cout << "command: "; 
	std::getline(std::cin ,in_str);

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

void logger::print_tests(std::map<u64 , test> const& tests) {
	std::cout << "Ungrouped Tests: " << tests.size() << " tests\n";

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" });

	table[0].format().font_color(Color::cyan);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	for ( std::pair<u64, test> const& _test : tests) {
		table.add_row(RowStream{} << _test.second.get_id() << _test.second.get_test_name());
	}

	std::cout << table << "\n";
}

void logger::print_group(group const& _group) {

	std::cout << "Group   : " << _group.get_name() << "\n";
	std::cout << "ID      : " << _group.get_id() << "\n";
	std::cout << "Include : " << _group.get_tests_list().size() << " tests\n";

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" });

	table[0].format().font_color(Color::cyan);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	for (test const& _test : _group.get_tests_list()) {
		table.add_row(RowStream{} << _test.get_id() << _test.get_test_name());
	}

	std::cout << table << "\n";
}

void logger::print_groups(std::map<u64 , group> const& groups){

	for (std::pair<u64 , group> const& _group : groups) {
		logger::print_group( _group.second );
	}

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
	table.add_row({ "ID", "Test-Name" , "Execute-Time" , "Test-Result"});

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	for (u32 i = 1; i < size; i++) {
		test_result test = _test[i].get_test_result();

		table.add_row(
			RowStream{} << 
			std::to_string(test.id) << _test[i].get_test_name() <<
			(test.last_exec_time > 0 ? (std::to_string(test.last_exec_time) + " ns") : "uncounted") <<
			(test.success ? "successed" : "failed")
		);

		table[i].format().font_background_color( test.success ? Color::green : Color::red);	
	}

	std::cout << table << "\n";
}

void logger::print_tests_results(std::map<u64 , test>& tests) {

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" , "Execute-Time" , "Test-Result"});

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);
	
	u32 i = 0;
	for (std::pair<u64 , test> const& pair : tests) {
		test const& _test = pair.second;
		test_result _test_result = _test.get_test_result();
		
		table.add_row(
			RowStream{} << 
			std::to_string(_test_result.id) << _test.get_test_name() <<
			(_test_result.last_exec_time > 0 ? (std::to_string(_test_result.last_exec_time) + " ns") : "uncounted") <<
			(_test_result.success ? "successed" : "failed")
		);

		table[i+1].format().font_background_color( _test_result.success ? Color::green : Color::red);	
		
		i++;
	}

	std::cout << table << "\n";
}

void logger::print_group_results(group const& _group) {

	std::cout << "Group   : " << _group.get_name() << "\n";
	std::cout << "ID      : " << _group.get_id()   << "\n";
	std::cout << "Include : " << _group.get_tests_list().size() << "\n";

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" , "Execute-Time" , "Test-Result"});

	table[0].format().font_color(Color::yellow);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	std::vector<test> const& tests = _group.get_tests_list();
	u64 size = tests.size();

	for (u32 i = 0; i < size; i++) {
		test_result test = tests[i].get_test_result();

		table.add_row(
			RowStream{} << 
			std::to_string(test.id) << tests[i].get_test_name() <<
			(test.last_exec_time > 0 ? (std::to_string(test.last_exec_time) + " ns") : "uncounted") <<
			(test.success ? "successed" : "failed")
		);

		table[i+1].format().font_background_color( test.success ? Color::green : Color::red);	
	}

	std::cout << table << "\n";
	std::cout << "Group          : " << _group.get_name() << "\n";
	std::cout << "ID             : " << _group.get_id()   << "\n";
	std::cout << "Execution-Time : " << ((group&)_group).get_exec_time() << "ns \n";
}

void inline print_msg(std::string const& title , std::string const& message , Color color) {
	Table table;

	table.add_row({ title + message });
	table.format()	
		.font_color(color)
		.border_top("")
		.border_bottom("")
		.border_left("")
		.border_right("")
		.corner("");

	std::cout << table << "\n";
}

void logger::hint(std::string const& hint_message) {
	print_msg("Hint: ", hint_message, Color::green);
}

void logger::warn(std::string const& warn_message) {
	print_msg("Warn: ", warn_message, Color::yellow);
}

void logger::error(std::string const& error_message) {
	print_msg("Error: ", error_message, Color::red);
}

void logger::print_help() {
	std::cout << "\nCommands :\n";
	std::cout << "exit: stop program;\n";
	std::cout << "save: save result in tester_logs folder;\n";
	std::cout << "list: show all current tests and groups;\n";
	std::cout << "exec_test  'id': run a test using test id;\n";
	std::cout << "exec_group 'id': run a group of tests using group id;\n";
	std::cout << "exec_all       : run all the tests;\n";
}

#endif