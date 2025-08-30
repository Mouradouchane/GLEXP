#pragma once 

#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>
#include <ctime> // Required for time functions
#include <iomanip> // Required for std::put_times
#include <string>

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::nanoseconds nano_second;

inline void generate_file_name(std::string& str);

#define COUNT_TEST_EXEC_TIME(FUNCTION , PTR_TEST) {\
				time_point start_time = std::chrono::high_resolution_clock::now();\
				PTR_TEST->last_exec_result = FUNCTION();\
				time_point end_time  = std::chrono::high_resolution_clock::now();\
				\
				nano_second duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);\
				PTR_TEST->last_exec_time = u64(duration.count());\
		};

#define COUNT_GROUP_EXEC_TIME(PTR_GROUP) {\
				time_point start_time = std::chrono::high_resolution_clock::now();\
				for (test& _test : PTR_GROUP->tests) {\
					_test.run_test(true);\
				}\
				time_point end_time  = std::chrono::high_resolution_clock::now();\
				\
				nano_second duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);\
				PTR_GROUP->last_exec_time = u64(duration.count());\
		};


inline void generate_file_name(std::string& output) {
	// create a date-time string
	time_t now = time(0); 
	tm* local_time = new tm();
	
	localtime_s(local_time, &now);

	char time_buffer[128];
	std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d_%H-%M-%S", local_time);
	std::string formatted_date_time_string(time_buffer);

	// create new log file
	output = ("results_" + formatted_date_time_string + ".log");

	delete local_time;
}

#endif