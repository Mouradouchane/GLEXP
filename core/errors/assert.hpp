#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include "core/macros.hpp"
#include "core/errors/errors.hpp"
#include "core/logger/logger.hpp"

/*
	compile-time assert
*/

#define COMPILE_TIME_ASSERT( CONST_EXPRESSION, MESSAGE ) \
		static_assert( CONST_EXPRESSION , MESS AGE );

/*
	run-time assert
*/

/*
	TODO: implememnt better ways of assert !!!!!!!!!!!!!!!!!!!!
*/

#ifdef DEBUG

	#define ASSERT_ERR( EXPRESSION , ASSERT_MESSAGE ) \
				if( EXPRESSION != ERR::NO_ERR ) { CORE_FATAL(ASSERT_MESSAGE); DEBUG_BREAK; CORE_CRASH(); } 

	#define ASSERT_EXP( TRUE_CONDITION , ASSERT_MESSAGE) \
				if(TRUE_CONDITION) { CORE_FATAL(ASSERT_MESSAGE); DEBUG_BREAK; CORE_CRASH(); } 

#else 
	#define ASSERT_ERR( EXPRESSION , ASSERT_MESSAGE ) \
			if( EXPRESSION != ERR::NO_ERR ) CORE_CRASH();

	#define ASSERT_EXP( TRUE_CONDITION , ASSERT_MESSAGE ) \
				if(TRUE_CONDITION) CORE_CRASH();

#endif 

#endif
