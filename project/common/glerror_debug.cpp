#pragma once

#ifndef GL_ERRORS_DEBUG_CPP
#define GL_ERRORS_DEBUG_CPP

#include <stdint.h>
#include "glew/glew.h"
#include "glerror_debug.hpp"

void clear_opengl_errors() {
	while (glGetError() != GL_NO_ERROR);
}

void handle_opengl_errors(
	uint32_t    _line , 
	const char* _function ,
	const char* _file
) {
	GLenum err;

	while ( (err = glGetError()) != GL_NO_ERROR) {
		// TODO: show messagebox include gl error
	}
}


#endif