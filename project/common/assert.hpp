
#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "errors.hpp"

#ifdef DEBUG
	#define ASSERT( EXPRESSION ) if( EXPRESSION != ERR::NO_ERR ) return 1; 	
	#define ASSERT_ERR( EXPRESSION ) if( EXPRESSION != ERR::NO_ERR ) return ERR::FAILED_WHILE_ASSERT; 	
#else 
	#define ASSERT( FUNCTION )
	#define ASSERT_ERR( EXPRESSION ) if( EXPRESSION != ERR::NO_ERR )  	
#endif

#endif
