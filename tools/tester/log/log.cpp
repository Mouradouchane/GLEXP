#pragma once 

#ifndef LOG_CPP
#define LOG_CPP

#include "libs/tabulate/table.hpp"
#include <iostream>
#include <fstream>
#include <ostream>
#include <filesystem> 

#include "tools/tester/time/time.hpp"
#include "tools/tester/string/string_utility.hpp"
#include "log.hpp"

using namespace tabulate;
namespace fs = std::filesystem;

// ========================================================================

std::string logger::get_user_input() {

	std::string in_str;
	std::cout << "\n";
	std::cout << "command: "; 
	std::getline(std::cin ,in_str);

	return in_str;
}

bool logger::save_tests_results_in_log_file( std::string const& results_as_str ) {
	
	if (results_as_str == "") return false;
	std::string logs_folder_path = "tester_logs";

	// if folder found
	if (!std::filesystem::exists(logs_folder_path) || !std::filesystem::is_directory(logs_folder_path)) {
		logger::warn("tester_logs folder not found !");
		
		// try to create folder for the logs
		if ( ! std::filesystem::create_directory(logs_folder_path)) {
			logger::error("failed to create tester_logs folder !");
			logger::error("failed to save tests results into .log file !");
			return false;
		}
	}

	logger::hint("tester_logs folder found !");

	std::string file_name = ""; generate_file_name(file_name);

	std::string file_path = "tester_logs/" + file_name;
	std::ofstream log_file( file_path );

	// try to write to log_file
	if (log_file.is_open()) { 
		logger::hint("writing results to " + file_name + " file.");
		log_file << results_as_str;
		log_file.close();
		return true;
	}
	else { // failed to write or open log_file
		logger::error("failed to open file or create " + file_name + " file !");
		logger::error("failed to save tests results into .log file !");
		return false;
	}

}

void logger::load_old_tests_from_files(std::map<std::string, old_test_result>& output_map) {
	
	fs::path tester_path = "./tester_logs";
	
	if ( ! fs::is_directory(tester_path)) {
		logger::warn("tester_logs folder not found !");
		return;
	}
	
	logger::hint("searching for log files ...");
	std::vector<std::string> log_files_list;
	for (fs::directory_entry const& entry : fs::directory_iterator(tester_path)) {

		// get all .log files names 
		if (fs::is_regular_file(entry.status())) {
			std::string file_name = entry.path().string();
			
			if ( ends_with(file_name, ".log") ) {
				log_files_list.push_back(file_name);
			}
		}
	}

	if (log_files_list.size() == 0) {
		logger::warn("no log file found !");
		return;
	}

	// try to load & parse old_results from log files
	for (std::string const& file_name : log_files_list) {

		std::ifstream log_file(file_name);
		if (log_file.is_open()) {
			std::string str_line;
			logger::hint("loading old results from file " + file_name);

			// load log file data
			while (std::getline(log_file, str_line)) {

				// parse + process data
				std::vector<std::string>* values = nullptr;
				values = split_string_by_space(str_line);

				if ((values != nullptr) && (values->size() >= 3)) {

					bool success = (*(values->begin() + 2) == "successed" ? true : false);
					if (!success) continue;

					u64 test_time    = string_to_u64(*(values->begin() + 1));
					std::string name = *(values->begin());

					auto& old_test_itr = output_map.find(*(values->begin()));

					// if aleary in the map
					if ((old_test_itr != output_map.end())) {

						if (old_test_itr->second.exec_time != 0 && old_test_itr->second.exec_time > test_time) {
							old_test_itr->second.exec_time = test_time;
						}
					}
					else {
						output_map.insert(
							{ name , old_test_result{ success , test_time } }
						);
					}

				}

				(values != nullptr) ? delete values : 0;
			}

		}
		else logger::error("failed to open file " + file_name);

		log_file.close();
		logger::hint("loading results from file " + file_name + " is done .");
	}

}

/*
	print tests/groups
*/

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


/*
	print results of tests/groups
*/

void logger::print_test_result(test const& _test , std::string& results_str, std::map<std::string,old_test_result>& old_results) {

	test_result result = _test.get_test_result();
	
	auto& old_test = old_results.find(_test.get_test_name());
	u64   old_time = (old_test == old_results.end()) ? 0u : old_test->second.exec_time;

	tabulate::Table table;
	table.add_row({ "ID" , "Test-Name" , "Execute-Time" , "Best-Time" , "Test-Result" });

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	table.add_row(
		RowStream{} << std::to_string(_test.get_id()) << _test.get_test_name() <<
		(result.last_exec_time > 0 ? (std::to_string(result.last_exec_time) + " ns") : "uncounted") <<
		(old_time > 0 ? std::to_string(old_time) + "ns" : "unkown" ) <<
		(result.success ? "successed" : "failed")
	);

	// log table
	table[1].format().font_background_color(result.success ? Color::green : Color::red);	
	std::cout << table << "\n";

	// save test if better time-record
	if (old_time > result.last_exec_time && result.success) {
		logger::hint("new time-record for test " + _test.get_test_name() + ", from " + std::to_string(old_time) + "ns to " + std::to_string(result.last_exec_time) + "ns good job .");
		old_test->second.success   = result.success;
		old_test->second.exec_time = result.last_exec_time;
	}

	// save results to string history
	results_str += test_to_string(_test);
}

void logger::print_tests_results(std::map<u64 , test>& tests , std::string& results_str, std::map<std::string,old_test_result>& old_results) {

	tabulate::Table table;
	table.add_row({ "ID", "Test-Name" , "Execute-Time" , "Best-Time" , "Test-Result"});

	table[0].format().font_color(Color::magenta);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);
	
	u64 i = 0;
	for (std::pair<u64 , test> const& pair : tests) {
		// get current result and old result
		test const& _test = pair.second;
		test_result _test_result = _test.get_test_result();

		auto& old_test = old_results.find(_test.get_test_name());
		u64   old_time = (old_test == old_results.end()) ? 0u : old_test->second.exec_time;
		
		// setup row
		table.add_row(
			RowStream{} << 
			std::to_string(_test_result.id) << _test.get_test_name() <<
			(_test_result.last_exec_time > 0 ? (std::to_string(_test_result.last_exec_time) + " ns") : "uncounted") <<
			(old_time > 0 ? std::to_string(old_time)+ "ns" : "unkown" ) <<
			(_test_result.success ? "successed" : "failed")
		);

		// save test if better time-record
		if (old_time > _test_result.last_exec_time && _test_result.success) {
			logger::hint("new time-record for test " + _test.get_test_name() + ", from " + std::to_string(old_time) + "ns to " + std::to_string(_test_result.last_exec_time) + "ns good job .");
			old_test->second.success   = _test_result.success;
			old_test->second.exec_time = _test_result.last_exec_time;
		}

		table[i+1].format().font_background_color(_test_result.success ? Color::green : Color::red);	
		// log to the table
		results_str += test_to_string(_test);
		i++;
	}
	
	std::cout << table << "\n";
}

void logger::print_group_results(group const& _group, std::string& results_str, std::map<std::string,old_test_result>& old_results) {

	std::cout << "===================================\n";
	std::cout << "Group   : " << _group.get_name() << "\n";
	std::cout << "ID      : " << _group.get_id()   << "\n";
	std::cout << "Include : " << _group.get_tests_list().size() << "\n";
	std::cout << "===================================\n";

	tabulate::Table table;
	table.add_row({ "ID" , "Test-Name" , "Execute-Time" , "Best-Time" , "Test-Result" });

	table[0].format().font_color(Color::yellow);
	table[0].format().font_style({FontStyle::bold});
	table.format().font_align(FontAlign::center);

	std::vector<test> const& tests = _group.get_tests_list();
	u64 size = tests.size();

	for (size_t i = 0; i < size; i++) {
		test_result result = tests[i].get_test_result();

		auto& old_test = old_results.find( tests[i].get_test_name());
		u64   old_time = (old_test == old_results.end()) ? 0u : old_test->second.exec_time;

		table.add_row(
			RowStream{} << 
			std::to_string(result.id) << tests[i].get_test_name() <<
			(result.last_exec_time > 0 ? (std::to_string(result.last_exec_time) + " ns") : "uncounted") <<
			(old_time > 0 ? std::to_string(old_time)+ "ns" : "unkown") <<
			(result.success ? "successed" : "failed")
		);

		// save test if better time-record
		if (old_time > result.last_exec_time && result.success) {
			logger::hint("new time-record for test " + tests[i].get_test_name() + ", from " + std::to_string(old_time) + "ns to " + std::to_string(result.last_exec_time) + "ns good job .");
			old_test->second.success   = result.success;
			old_test->second.exec_time = result.last_exec_time;
		}

		table[size_t(i+1)].format().font_background_color(result.success ? Color::green : Color::red);	
		
		// save results to string history
		results_str += test_to_string(tests[i]);
	}

	std::cout << table << "\n";
	std::cout << "===================================\n";
	std::cout << "Group          : " << _group.get_name() << "\n";
	std::cout << "ID             : " << _group.get_id()   << "\n";
	std::cout << "Execution-Time : " << ((group&)_group).get_exec_time() << "ns \n";
	std::cout << "===================================\n";

}


/*
	log functions
*/
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
	std::cout << "exit: save all current results and exit program;\n";
	std::cout << "save: save result in tester_logs folder;\n";
	std::cout << "list_all: show all current tests and groups;\n";
	std::cout << "list_tests: show all ungrouped tests;\n";
	std::cout << "list_groups: show all current groups;\n";
	std::cout << "clear_console: clear console from text;\n";
	std::cout << "clear_results: clear all tests results;\n";
	std::cout << "exec_test  'id1 , id2 , ...': run a test using test id;\n";
	std::cout << "exec_group 'id1 , id2 , ...': run a group of tests using group id;\n";
	std::cout << "exec_all: run all the tests;\n";
}

#endif