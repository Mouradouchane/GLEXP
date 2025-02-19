#pragma once

#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include "glew/glew.h"

#include "assert.hpp"
#include "errors.hpp"
#include "texture_2d.hpp"


// used to represent "vertex shader","fragement shader",...
struct shader_object{
	ERR last_error = ERR::NO_ERR;
	GLuint type = NULL;
	GLuint id   = NULL;
};

class shader {

public:
	ERR    last_error = ERR::NO_ERR;
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
