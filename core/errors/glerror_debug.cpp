#pragma once

#ifndef GL_ERRORS_DEBUG_CPP
#define GL_ERRORS_DEBUG_CPP

#include <stdint.h>
#include "libs/glew/glew.h"
#include "core/errors/glerror_debug.hpp"

void clear_opengl_errors() {
	GLenum err = glGetError();

	while ( err != GL_NO_ERROR ) {
		err = glGetError();
	}
}

void handle_opengl_errors(
	u32 _line , const char* _function , const char* _file
) {
	//bool result = false;
	GLenum err = glGetError();

	while ( err != GL_NO_ERROR ) {
		//result = true;
		// TODO: show messagebox include gl error

		#ifdef DEBUG
			__debugbreak();
		#endif
		err = glGetError();
	}
	// return result;
}

#endif