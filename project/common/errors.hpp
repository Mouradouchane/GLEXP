#pragma once

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdint.h>

enum class ERR : uint16_t {
	NO_ERR = 0,

	// assert errors
	ASSERT_STOP,

	// application errors
	FAILED_TO_INIT_APPLICATION,
	FAILED_TO_INIT_GLFW,
	FAILED_TO_CREATE_WINDOW,
	FAILED_TO_INIT_GLEW,

	// main loop errors
	INVALID_SHADER_PROGRAM,

	// "shader compile" errors
	FAILED_TO_COMPILE_VERTEX_SHADER,
	FAILED_TO_COMPILE_FRAGMENT_SHADER,

	// "shader linking" errors
	FAILED_TO_LINK_PROGRAM,

	// "shader create" errors
	FAILED_TO_CREATE_PROGRAM,
	FAILED_TO_CREATE_VERTEX_SHADER,
	FAILED_TO_CREATE_FRAGMENT_SHADER,

	// "files" errors
	FAILED_TO_LOAD_SHADER_FILE,

	// "textures" errors
	FAILED_TO_INIT_TEXTURES,
	FAILED_TO_LOAD_SOME_TEXTURE,

	// "configs" errors
	FAILED_TO_LOAD_INI_FILE,
	FAILED_TO_WRITE_TO_INI_FILE,
	UNSUPPORTED_RESOLUTION,
	UNSUPPORTED_FOV_VALUE,
};

#endif
