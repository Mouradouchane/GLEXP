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

	#define CORE_ASSERT_ERR(ERROR_ENUM , ASSERT_MESSAGE) \
			if(ERROR_ENUM != core::error::none) {\
				CORE_FATAL("ASSERT_ERROR:{}", ASSERT_MESSAGE); \
				CORE_TRACE("CODE:{}", #ERROR_ENUM); \
				CORE_TRACE("FILE:{}", __FILE__); \
				CORE_TRACE("LINE:{}", __LINE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

	#define CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE) \
			if(TRUE_CONDITION) {\
				CORE_FATAL("ASSERT_:{}", ASSERT_MESSAGE); \
				CORE_TRACE("CODE:{}", #TRUE_CONDITION); \
				CORE_TRACE("FILE:{}", __FILE__); \
				CORE_TRACE("LINE:{}", __LINE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

	#define CRASH_IF(TRUE_CONDITION , CRASH_MESSAGE) \
			if(TRUE_CONDITION) {\
				CORE_FATAL("CRASH_IF:{}", CRASH_MESSAGE); \
				CORE_TRACE("CODE:{}", #TRUE_CONDITION); \
				CORE_TRACE("FILE:{}", __FILE__); \
				CORE_TRACE("LINE:{}", __LINE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

	#define CRASH_IF_ERROR(ERROR_ENUM , CRASH_MESSAGE) \
				if(ERROR_ENUM != core::error::none) {\
					CORE_FATAL("CRASH_IF_ERROR:{}", CRASH_MESSAGE); \
					CORE_TRACE("CODE:{}", #ERROR_ENUM); \
					CORE_TRACE("FILE:{}", __FILE__); \
					CORE_TRACE("LINE:{}", __LINE__); \
					DEBUG_BREAK; \
					CORE_CRASH(); \
				}

#elif defined(ALLOW_CRASH_REPORT)

	#define CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE) \
			if(TRUE_CONDITION) {\
				CORE_FATAL("ERROR:{} , FILE:{} , LINE:{}", ASSERT_MESSAGE , __FILE__ , __LINE__); \
				CORE_CRASH(); \
			}

	#define CORE_ASSERT_ERR(ERROR_ENUM , ASSERT_MESSAGE) \
			if(ERROR_ENUM != core::error::none) {\
				CORE_FATAL("ERROR:{} , FILE:{} , LINE:{}", ASSERT_MESSAGE , __FILE__ , __LINE__); \
				CORE_CRASH(); \
			}

	#define CRASH_IF(TRUE_CONDITION , CRASH_MESSAGE) \
			if(TRUE_CONDITION) exit(-1) {\
				CORE_FATAL("ERROR:{} , FILE:{} , LINE:{}", CRASH_MESSAGE , __FILE__ , __LINE__); \
				CORE_CRASH(); \
			}

	#define CRASH_IF_ERROR(ERROR_ENUM , CRASH_MESSAGE) 
			if(ERROR_ENUM != core::error::none){\
				CORE_FATAL("ERROR:{} , FILE:{} , LINE:{}", CRASH_MESSAGE , __FILE__ , __LINE__); \
				CORE_CRASH(); \
			}

#else
	#define CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE)   if(TRUE_CONDITION) exit(-1);
	#define CORE_ASSERT_ERR(ERROR_ENUM , ASSERT_MESSAGE)  if(ERROR_ENUM != core::error::none) exit(-1);
	#define CRASH_IF(TRUE_CONDITION , CRASH_MESSAGE)   if(TRUE_CONDITION) exit(-1);
	#define CRASH_IF_ERROR(ERROR_ENUM , CRASH_MESSAGE) if(ERROR_ENUM != core::error::none) exit(-1);

#endif 

#endif
