#pragma once 

#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::nanoseconds nano_second;

#define COUNT_TEST_EXEC_TIME(FUNCTION , PTR_TEST) {\
				time_point start_time = std::chrono::high_resolution_clock::now();\
				PTR_TEST->last_exec_result = FUNCTION();\
				time_point end_time = std::chrono::high_resolution_clock::now();\
				\
				nano_second duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);\
				PTR_TEST->last_exec_time = u64(duration.count());\
		};

#define COUNT_GROUP_EXEC_TIME(PTR_GROUP) {\
				time_point start_time = std::chrono::high_resolution_clock::now();\
				for (test& _test : PTR_GROUP->tests) {\
					_test.run_test(true);\
				}\
				time_point end_time = std::chrono::high_resolution_clock::now();\
				\
				nano_second duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);\
				PTR_GROUP->last_exec_time = u64(duration.count());\
		};

#endif