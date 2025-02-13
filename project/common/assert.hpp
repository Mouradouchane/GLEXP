#pragma once 

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cstdlib>
#include "errors.hpp"

#define ASSERT( EXPRESSION ) \
		if( EXPRESSION != ERR::NO_ERR ) exit(-1); 

#define ASSERT_V( EXPRESSION ) \
		if( ! EXPRESSION ) exit(-1); 

#define ASSERT_APP_INIT( EXPRESSION ) \
		if( EXPRESSION != ERR::NO_ERR ) return ERR::FAILED_TO_INIT_APPLICATION;

// TODO : implement error/warn messagebox macro functions

#endif
