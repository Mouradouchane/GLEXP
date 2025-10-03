#pragma once

#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include "libs/glew/glew.h"
#include "core/errors/errors.hpp"

// used to represent "vertex shader","fragment shader",...
struct shader_object{
	core::error last_error = core::error::none;
	GLuint type = NULL;
	GLuint id   = NULL;
};

typedef shader_object vertex_shader;
typedef shader_object fragment_shader;

class shader {

public:
	core::error last_error = core::error::none;
	GLuint id = NULL; // "shader program" id

	shader(std::string const& shader_code, std::string const& fragement_shader_path);
	~shader();

	void use();

	GLint get_uniform_location(std::string const& uniform_name);

	void   set_int(GLint uniform_location, int value);
	void  set_bool(GLint uniform_location, bool value);
	void set_float(GLint uniform_location, float value);
};

#endif
