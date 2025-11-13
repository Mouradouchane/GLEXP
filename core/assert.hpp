#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include <string>

#include "core/macros.hpp"
#include "core/status/status.hpp"
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
		
		DEBUG_BREAK;
		
		assert(expression);
	}

}

#if defined(DEBUG) or defined(ALLOW_CRASH_REPORT)

	#define CORE_ASSERT_ERR(ERROR_ENUM , ASSERT_MESSAGE , ...) \
			if(ERROR_ENUM != core::error::none) {\
				CORE_FATAL(ASSERT_MESSAGE , __VA_ARGS__); \
				CORE_TRACE("CODE:{}", #ERROR_ENUM); \
				CORE_TRACE("FILE:{}", __FILE__); \
				CORE_TRACE("LINE:{}", __LINE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

	#define CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE ) \
			if(TRUE_CONDITION) {\
				CORE_FATAL(ASSERT_MESSAGE); \
				CORE_TRACE("CODE:{}", #TRUE_CONDITION); \
				CORE_TRACE("LINE:{}", __LINE__); \
				CORE_TRACE("FILE:{}", __FILE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

	#define VCORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE , ...) \
				if(TRUE_CONDITION) {\
					CORE_FATAL(ASSERT_MESSAGE , __VA_ARGS__); \
					CORE_TRACE("CODE:{}", #TRUE_CONDITION); \
					CORE_TRACE("LINE:{}", __LINE__); \
					CORE_TRACE("FILE:{}", __FILE__); \
					DEBUG_BREAK; \
					CORE_CRASH(); \
				}

	#define VCRASH_IF(TRUE_CONDITION , CRASH_MESSAGE, ...) \
				if(TRUE_CONDITION) {\
					CORE_FATAL(CRASH_MESSAGE , __VA_ARGS__); \
					CORE_TRACE("CODE:{}", #TRUE_CONDITION); \
					CORE_TRACE("LINE:{}", __LINE__); \
					CORE_TRACE("FILE:{}", __FILE__); \
					DEBUG_BREAK; \
					CORE_CRASH(); \
				}

	#define CRASH_IF(TRUE_CONDITION , CRASH_MESSAGE) \
				if(TRUE_CONDITION) {\
					CORE_FATAL(CRASH_MESSAGE); \
					CORE_TRACE("CODE:{}", #TRUE_CONDITION); \
					CORE_TRACE("LINE:{}", __LINE__); \
					CORE_TRACE("FILE:{}", __FILE__); \
					DEBUG_BREAK; \
					CORE_CRASH(); \
				}

	#define CRASH_IF_ERROR(ERROR_ENUM , CRASH_MESSAGE) \
			if(ERROR_ENUM != core::error::none) {\
				CORE_FATAL(CRASH_MESSAGE); \
				CORE_TRACE("CODE:{}", #ERROR_ENUM); \
				CORE_TRACE("LINE:{}", __LINE__); \
				CORE_TRACE("FILE:{}", __FILE__); \
				DEBUG_BREAK; \
				CORE_CRASH(); \
			}

#else

	#define  CRASH_IF(TRUE_CONDITION    , CRASH_MESSAGE  , ...) if(TRUE_CONDITION) CORE_CRASH();
	#define VCRASH_IF(TRUE_CONDITION    , CRASH_MESSAGE)         if(TRUE_CONDITION) CORE_CRASH();
	#define  CRASH_IF_ERROR(ERROR_ENUM  , CRASH_MESSAGE  , ...) if(ERROR_ENUM != core::error::none) CORE_CRASH();
	#define  CORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE)       if(TRUE_CONDITION) CORE_CRASH();
	#define VCORE_ASSERT(TRUE_CONDITION , ASSERT_MESSAGE , ...) if(TRUE_CONDITION) CORE_CRASH();
	#define  CORE_ASSERT_ERR(ERROR_ENUM , ASSERT_MESSAGE , ...) if(ERROR_ENUM != core::error::none) CORE_CRASH();

#endif 

#endif
