#pragma once 

#include "core/macros.hpp"

#ifndef TESTER_MACROS
#define TESTER_MACROS

#define TEST_FAIL false
#define TEST_PASS true

#define DEBUG_FAILUER() \
		DEBUG_BREAK; \
		return TEST_FAIL;

#endif