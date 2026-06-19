#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include <string>

#include "core/macros.hpp"
#include "core/logger/logger.hpp"

/*
	compile-time assert
*/

#define COMPILE_TIME_ASSERT(TRUE_CONST_EXPRESSION, MESSAGE) static_assert(!TRUE_CONST_EXPRESSION, MESSAGE);

/*
	run-time assert
*/

#ifdef DEBUG

	#define CORE_ASSERT(TRUE_CONDITION , FORMAT , ...) \
				if(TRUE_CONDITION) {\
					CORE_FATAL_F(FORMAT , ##__VA_ARGS__); \
					CORE_TRACE("condition:{}", #TRUE_CONDITION); \
					CORE_CRASH(); \
				}

	#define CRASH_IF(TRUE_CONDITION , FORMAT , ...) \
				if(TRUE_CONDITION) { \
					CORE_FATAL_F(FORMAT , ##__VA_ARGS__); \
					CORE_TRACE("condition:{}", #TRUE_CONDITION); \
					CORE_CRASH(); \
				}

#else

	#define  CRASH_IF(TRUE_CONDITION    , FORMAT , ...) if(TRUE_CONDITION) CORE_CRASH();
	#define  CORE_ASSERT(TRUE_CONDITION , FORMAT , ...) if(TRUE_CONDITION) CORE_CRASH();

#endif 

#endif
