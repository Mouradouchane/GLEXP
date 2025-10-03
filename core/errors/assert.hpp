#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include <string>

#include "core/macros.hpp"
#include "core/errors/errors.hpp"
#include "core/logger/logger.hpp"

/*
	compile-time assert
*/

#define COMPILE_TIME_ASSERT(CONST_EXPRESSION, MESSAGE) static_assert(CONST_EXPRESSION, MESSAGE);

/*
	run-time assert
*/

namespace core {

	inline void assert_(bool expression, std::string const& message) {
		CORE_FATAL(message);
	#ifdef DEBUG
		DEBUG_BREAK;
	#endif
		assert(expression);
	}

}

#ifdef DEBUG

	#define CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE) \
			if(TRUE_CONDITION) {\
				CORE_FATAL(ASSERT_MESSAGE); \
				CORE_TRACE("CONDITION:{}", #TRUE_CONDITION); \
				CORE_TRACE("FILE:{}", __FILE__); \
				CORE_TRACE("LINE:{}", __LINE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

	#define CRASH_IF(TRUE_CONDITION , ASSERT_MESSAGE) \
			if(TRUE_CONDITION) {\
				CORE_FATAL(ASSERT_MESSAGE); \
				CORE_TRACE("CONDITION:{}", #TRUE_CONDITION); \
				CORE_TRACE("FILE:{}", __FILE__); \
				CORE_TRACE("LINE:{}", __LINE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}
#else 
	// todo: log crash info to files
	#define CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE) if(TRUE_CONDITION) exit(-1);
	#define CRASH_IF(TRUE_CONDITION    , ASSERT_MESSAGE) if(TRUE_CONDITION) exit(-1);
			
#endif 

#endif
