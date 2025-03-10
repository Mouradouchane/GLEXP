#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include "errors.hpp"

/*
	compile-time assert
*/

#define COMPILE_TIME_ASSERT( CONST_EXPRESSION, MESSAGE ) \
		static_assert( CONST_EXPRESSION , MESSAGE );

/*
	run-time assert
*/
#define ASSERT_ERR( EXPRESSION ) \
		if( EXPRESSION != ERR::NO_ERR ) exit(-1); 

#define ASSERT_EXP( EXPRESSION ) \
		if( ! EXPRESSION ) exit(-1); 

#define ASSERT_APP_INIT( EXPRESSION ) \
		if( EXPRESSION != ERR::NO_ERR ) return ERR::FAILED_TO_INIT_APPLICATION;

#endif
