#pragma once

/*
	for opengl error handling and debug
*/

#ifndef GL_ERRORS_DEBUG_HPP
#define GL_ERRORS_DEBUG_HPP

#ifdef DEBUG

void clear_opengl_errors();
void handle_opengl_errors(
		uint32_t    _line, 
		const char* _function, 
		const char* _file
);

#define GL_CHECK( GL_FUNCTION ) \
	clear_opengl_errors(); \
	(GL_FUNCTION); \
	handle_opengl_errors(__LINE__, #GL_FUNCTION, __FILE__);

#else
	#define GL_CHECK( GL_FUNCTION )

#endif

#endif