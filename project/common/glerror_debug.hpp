#pragma once

#ifndef GL_ERRORS_DEBUG_HPP
#define GL_ERRORS_DEBUG_HPP

#ifdef DEBUG

void clear_opengl_errors();
void handle_opengl_errors(
		uint32_t    _line , 
		const char* _function, 
		const char* _file
);

#define GL_CHECK( CALLED_FUNCTION ) \
	clear_opengl_errors(); \
	(CALLED_FUNCTION); \
	handle_opengl_errors(__LINE__, #CALLED_FUNCTION, __FILE__);

#else
	#define GL_CHECK( CALLED_FUNCTION ) 

#endif

#endif