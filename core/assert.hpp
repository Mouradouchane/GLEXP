#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include "macros.hpp"
#include "core/errors/errors.hpp"

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

	#define ASSERT_ERR( EXPRESSION ) \
				if( EXPRESSION != ERR::NO_ERR ) { DEBUG_BREAK; exit(-1); } 

	#define ASSERT_EXP( EXPRESSION ) \
				if( ! EXPRESSION ) { DEBUG_BREAK; exit(-1); } 

#else 
	#define ASSERT_ERR( EXPRESSION ) \
			if( EXPRESSION != ERR::NO_ERR ) exit(-1); 

	#define ASSERT_EXP( EXPRESSION ) \
			if( ! EXPRESSION ) exit(-1); 

#endif 

#define ASSERT_APP_INIT( EXPRESSION ) \
		if( EXPRESSION != ERR::NO_ERR ) return ERR::FAILED_TO_INIT_APPLICATION;


#endif
